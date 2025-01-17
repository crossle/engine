// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "flutter/display_list/display_list_color_filter.h"
#include "flutter/display_list/types.h"
#include "gtest/gtest.h"

namespace flutter {
namespace testing {

static const float matrix[20] = {
    1,  2,  3,  4,  5,   //
    6,  7,  8,  9,  10,  //
    11, 12, 13, 14, 15,  //
    16, 17, 18, 19, 20,  //
};

TEST(DisplayListColorFilter, FromSkiaNullFilter) {
  std::shared_ptr<DlColorFilter> filter = DlColorFilter::From(nullptr);
  ASSERT_EQ(filter, nullptr);
  ASSERT_EQ(filter.get(), nullptr);
}

TEST(DisplayListColorFilter, FromSkiaBlendFilter) {
  sk_sp<SkColorFilter> sk_filter =
      SkColorFilters::Blend(SK_ColorRED, SkBlendMode::kDstATop);
  std::shared_ptr<DlColorFilter> filter = DlColorFilter::From(sk_filter);
  DlBlendColorFilter dl_filter(SK_ColorRED, SkBlendMode::kDstATop);
  ASSERT_EQ(filter->type(), DlColorFilter::kBlend);
  ASSERT_NE(filter->asBlend(), nullptr);
  ASSERT_EQ(filter->asMatrix(), nullptr);
  ASSERT_EQ(*filter->asBlend(), dl_filter);
  ASSERT_EQ(filter->asBlend()->color(), SK_ColorRED);
  ASSERT_EQ(filter->asBlend()->mode(), SkBlendMode::kDstATop);
}

TEST(DisplayListColorFilter, FromSkiaMatrixFilter) {
  sk_sp<SkColorFilter> sk_filter = SkColorFilters::Matrix(matrix);
  std::shared_ptr<DlColorFilter> filter = DlColorFilter::From(sk_filter);
  DlMatrixColorFilter dl_filter(matrix);
  ASSERT_EQ(filter->type(), DlColorFilter::kMatrix);
  ASSERT_EQ(filter->asBlend(), nullptr);
  ASSERT_NE(filter->asMatrix(), nullptr);
  ASSERT_EQ(*filter->asMatrix(), dl_filter);
  const DlMatrixColorFilter* matrix_filter = filter->asMatrix();
  for (int i = 0; i < 20; i++) {
    ASSERT_EQ((*matrix_filter)[i], matrix[i]);
  }
}

TEST(DisplayListColorFilter, FromSkiaSrgbToLinearFilter) {
  sk_sp<SkColorFilter> sk_filter = SkColorFilters::SRGBToLinearGamma();
  std::shared_ptr<DlColorFilter> filter = DlColorFilter::From(sk_filter);
  ASSERT_EQ(filter->type(), DlColorFilter::kSrgbToLinearGamma);
  ASSERT_EQ(filter->asBlend(), nullptr);
  ASSERT_EQ(filter->asMatrix(), nullptr);
}

TEST(DisplayListColorFilter, FromSkiaLinearToSrgbFilter) {
  sk_sp<SkColorFilter> sk_filter = SkColorFilters::LinearToSRGBGamma();
  std::shared_ptr<DlColorFilter> filter = DlColorFilter::From(sk_filter);
  ASSERT_EQ(filter->type(), DlColorFilter::kLinearToSrgbGamma);
  ASSERT_EQ(filter->asBlend(), nullptr);
  ASSERT_EQ(filter->asMatrix(), nullptr);
}

TEST(DisplayListColorFilter, FromSkiaUnrecognizedFilter) {
  sk_sp<SkColorFilter> sk_inputA =
      SkColorFilters::Blend(SK_ColorRED, SkBlendMode::kOverlay);
  sk_sp<SkColorFilter> sk_inputB =
      SkColorFilters::Blend(SK_ColorBLUE, SkBlendMode::kScreen);
  sk_sp<SkColorFilter> sk_filter =
      SkColorFilters::Compose(sk_inputA, sk_inputB);
  std::shared_ptr<DlColorFilter> filter = DlColorFilter::From(sk_filter);
  ASSERT_EQ(filter->type(), DlColorFilter::kUnknown);
  ASSERT_EQ(filter->asBlend(), nullptr);
  ASSERT_EQ(filter->asMatrix(), nullptr);
  ASSERT_EQ(filter->sk_filter(), sk_filter);
}

TEST(DisplayListColorFilter, BlendConstructor) {
  DlBlendColorFilter filter(SK_ColorRED, SkBlendMode::kDstATop);
}

TEST(DisplayListColorFilter, BlendShared) {
  DlBlendColorFilter filter(SK_ColorRED, SkBlendMode::kDstATop);
  ASSERT_NE(filter.shared().get(), &filter);
  ASSERT_EQ(*filter.shared(), filter);
}

TEST(DisplayListColorFilter, BlendAsBlend) {
  DlBlendColorFilter filter(SK_ColorRED, SkBlendMode::kDstATop);
  ASSERT_NE(filter.asBlend(), nullptr);
  ASSERT_EQ(filter.asBlend(), &filter);
}

TEST(DisplayListColorFilter, BlendContents) {
  DlBlendColorFilter filter(SK_ColorRED, SkBlendMode::kDstATop);
  ASSERT_EQ(filter.color(), SK_ColorRED);
  ASSERT_EQ(filter.mode(), SkBlendMode::kDstATop);
}

TEST(DisplayListColorFilter, BlendEquals) {
  DlBlendColorFilter filter1(SK_ColorRED, SkBlendMode::kDstATop);
  DlBlendColorFilter filter2(SK_ColorRED, SkBlendMode::kDstATop);
  ASSERT_TRUE(filter1 == filter2);
  ASSERT_TRUE(filter2 == filter1);
  ASSERT_FALSE(filter1 != filter2);
  ASSERT_FALSE(filter2 != filter1);
  ASSERT_EQ(filter1, filter2);
}

TEST(DisplayListColorFilter, BlendNotEquals) {
  DlBlendColorFilter filter1(SK_ColorRED, SkBlendMode::kDstATop);
  DlBlendColorFilter filter2(SK_ColorBLUE, SkBlendMode::kDstATop);
  DlBlendColorFilter filter3(SK_ColorRED, SkBlendMode::kDstIn);
  ASSERT_FALSE(filter1 == filter2);
  ASSERT_FALSE(filter2 == filter1);
  ASSERT_TRUE(filter1 != filter2);
  ASSERT_TRUE(filter2 != filter1);
  ASSERT_NE(filter1, filter2);
  ASSERT_NE(filter2, filter3);
  ASSERT_NE(filter3, filter1);
}

TEST(DisplayListColorFilter, MatrixConstructor) {
  DlMatrixColorFilter filter(matrix);
}

TEST(DisplayListColorFilter, MatrixShared) {
  DlMatrixColorFilter filter(matrix);
  ASSERT_NE(filter.shared().get(), &filter);
  ASSERT_EQ(*filter.shared(), filter);
}

TEST(DisplayListColorFilter, MatrixAsMatrix) {
  DlMatrixColorFilter filter(matrix);
  ASSERT_NE(filter.asMatrix(), nullptr);
  ASSERT_EQ(filter.asMatrix(), &filter);
}

TEST(DisplayListColorFilter, MatrixContents) {
  float matrix_[20];
  memcpy(matrix_, matrix, sizeof(matrix_));
  DlMatrixColorFilter filter(matrix_);

  // Test deref operator []
  for (int i = 0; i < 20; i++) {
    ASSERT_EQ(filter[i], matrix_[i]);
  }

  // Test get_matrix
  float matrix2[20];
  filter.get_matrix(matrix2);
  for (int i = 0; i < 20; i++) {
    ASSERT_EQ(matrix2[i], matrix_[i]);
  }

  // Test perturbing original array does not affect filter
  float original_value = matrix_[4];
  matrix_[4] += 101;
  ASSERT_EQ(filter[4], original_value);
}

TEST(DisplayListColorFilter, MatrixEquals) {
  DlMatrixColorFilter filter1(matrix);
  DlMatrixColorFilter filter2(matrix);
  ASSERT_EQ(filter1, filter2);
}

TEST(DisplayListColorFilter, MatrixNotEquals) {
  float matrix_[20];
  memcpy(matrix_, matrix, sizeof(matrix_));
  DlMatrixColorFilter filter1(matrix_);
  matrix_[4] += 101;
  DlMatrixColorFilter filter2(matrix_);
  ASSERT_NE(filter1, filter2);
}

TEST(DisplayListColorFilter, SrgbToLinearConstructor) {
  DlSrgbToLinearGammaColorFilter filter;
}

TEST(DisplayListColorFilter, SrgbToLinearShared) {
  DlSrgbToLinearGammaColorFilter filter;
  ASSERT_NE(filter.shared().get(), &filter);
  ASSERT_EQ(*filter.shared(), filter);
}

TEST(DisplayListColorFilter, SrgbToLinearEquals) {
  DlSrgbToLinearGammaColorFilter filter1;
  DlSrgbToLinearGammaColorFilter filter2;
  ASSERT_TRUE(filter1 == filter2);
  ASSERT_TRUE(filter2 == filter1);
  ASSERT_FALSE(filter1 != filter2);
  ASSERT_FALSE(filter2 != filter1);
  ASSERT_EQ(filter1, filter2);
  ASSERT_EQ(filter1, *DlSrgbToLinearGammaColorFilter::instance);
}

TEST(DisplayListColorFilter, LinearToSrgbConstructor) {
  DlLinearToSrgbGammaColorFilter filter;
}

TEST(DisplayListColorFilter, LinearToSrgbShared) {
  DlLinearToSrgbGammaColorFilter filter;
  ASSERT_NE(filter.shared().get(), &filter);
  ASSERT_EQ(*filter.shared(), filter);
}

TEST(DisplayListColorFilter, LinearToSrgbEquals) {
  DlLinearToSrgbGammaColorFilter filter1;
  DlLinearToSrgbGammaColorFilter filter2;
  ASSERT_TRUE(filter1 == filter2);
  ASSERT_TRUE(filter2 == filter1);
  ASSERT_FALSE(filter1 != filter2);
  ASSERT_FALSE(filter2 != filter1);
  ASSERT_EQ(filter1, filter2);
  ASSERT_EQ(filter1, *DlLinearToSrgbGammaColorFilter::instance);
}

TEST(DisplayListColorFilter, UnknownConstructor) {
  DlUnknownColorFilter filter(SkColorFilters::LinearToSRGBGamma());
}

TEST(DisplayListColorFilter, UnknownShared) {
  DlUnknownColorFilter filter(SkColorFilters::LinearToSRGBGamma());
  ASSERT_NE(filter.shared().get(), &filter);
  ASSERT_EQ(*filter.shared(), filter);
}

TEST(DisplayListColorFilter, UnknownContents) {
  sk_sp<SkColorFilter> sk_filter = SkColorFilters::LinearToSRGBGamma();
  DlUnknownColorFilter filter(sk_filter);
  ASSERT_EQ(sk_filter, filter.sk_filter());
  ASSERT_EQ(sk_filter.get(), filter.sk_filter().get());
}

TEST(DisplayListColorFilter, UnknownEquals) {
  sk_sp<SkColorFilter> sk_filter = SkColorFilters::LinearToSRGBGamma();
  DlUnknownColorFilter filter1(sk_filter);
  DlUnknownColorFilter filter2(sk_filter);
  ASSERT_TRUE(filter1 == filter2);
  ASSERT_TRUE(filter2 == filter1);
  ASSERT_FALSE(filter1 != filter2);
  ASSERT_FALSE(filter2 != filter1);
  ASSERT_EQ(filter1, filter2);
}

TEST(DisplayListColorFilter, UnknownNotEquals) {
  // Even though the filter is the same, it is a different instance
  // and we cannot currently tell them apart because the Skia
  // ColorFilter objects do not implement ==
  DlUnknownColorFilter filter1(
      SkColorFilters::Blend(SK_ColorRED, SkBlendMode::kDstATop));
  DlUnknownColorFilter filter2(
      SkColorFilters::Blend(SK_ColorRED, SkBlendMode::kDstATop));
  ASSERT_TRUE(filter1 != filter2);
  ASSERT_TRUE(filter2 != filter1);
  ASSERT_FALSE(filter1 == filter2);
  ASSERT_FALSE(filter2 == filter1);
  ASSERT_NE(filter1, filter2);
}

}  // namespace testing
}  // namespace flutter
