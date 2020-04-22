#pragma once

#ifndef TEST_CLASS_NAME
	#error "Test class name is not specified"
#endif

#include <gtest/gtest.h>

#define TEST_CLASS() \
	class TEST_CLASS_NAME : public ::testing::Test

#define UNIT_TEST(testName) \
	TEST_F(TEST_CLASS_NAME, testName)
