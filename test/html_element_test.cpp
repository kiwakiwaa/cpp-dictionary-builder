#include <gtest/gtest.h>
#include "yomitan_dictionary_builder/core/dictionary/dicentry.h"

TEST(HTMLElementTest, CreateElement)
{
    auto validElement = std::make_shared<HTMLElement>("span", "test content");
    validElement->setHref("maps.apple.com");

    EXPECT_EQ(validElement->getTag(), "span");
    EXPECT_EQ(validElement->getHref(), "maps.apple.com");

    // No validation should be done when creating elements
    EXPECT_NO_THROW(HTMLElement("見出しG"));
    EXPECT_NO_THROW(HTMLElement("details", "collapsible element content"));
}

