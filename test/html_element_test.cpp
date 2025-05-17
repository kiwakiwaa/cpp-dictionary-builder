#include <gtest/gtest.h>
#include "../src/core/html_element.h"
#include "../src/core/dicentry.h"

TEST(HTMLElementTest, CreateElement)
{
    const auto validElement = createHtmlElement("span", "test content", "maps.apple.com");
    EXPECT_EQ(validElement->getTag(), "span");
    EXPECT_EQ(validElement->getHref(), "maps.apple.com");

    // No validation should be done when creating elements
    EXPECT_NO_THROW(HTMLElement("見出しG"));
    EXPECT_NO_THROW(HTMLElement("details", "collapsible element content"));
}

