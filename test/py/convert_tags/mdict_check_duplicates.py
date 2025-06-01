#!/usr/bin/env python3
"""
Script to find duplicates in a text file with two types of entries:
1. Japanese text followed by @@@LINK=ID
2. Numbered entries followed by XML content

For duplicates to be found:
- Type 1: Both the text and LINK ID must match
- Type 2: The entire XML content must match (regardless of the number ID)
"""

import re
from collections import defaultdict
from typing import List, Tuple, Dict

def parse_file(filename: str) -> Tuple[List[Tuple[str, str]], List[Tuple[str, str]]]:
    """
    Parse the file and extract two types of entries:
    1. Text entries with LINK IDs
    2. Numbered entries with XML content

    Returns:
        Tuple of (text_entries, xml_entries)
        text_entries: List of (text, link_id) tuples
        xml_entries: List of (number_id, xml_content) tuples
    """
    text_entries = []
    xml_entries = []

    try:
        with open(filename, 'r', encoding='utf-8') as file:
            content = file.read()

        # Split by </> delimiter
        sections = content.split('</>')

        for section in sections:
            section = section.strip()
            if not section:
                continue

            # Check if it's a LINK entry (Japanese text + @@@LINK=ID)
            link_match = re.search(r'@@@LINK=(\d+)', section)
            if link_match:
                link_id = link_match.group(1)
                # Extract text before the @@@LINK
                text_part = section.split('@@@LINK=')[0].strip()
                text_entries.append((text_part, link_id))

            # Check if it's an XML entry (number + XML content)
            elif section.strip().isdigit() or re.match(r'^\d+\s*<\?xml', section):
                lines = section.split('\n', 1)
                if len(lines) >= 2:
                    number_id = lines[0].strip()
                    xml_content = lines[1].strip() if len(lines) > 1 else ""
                    if xml_content.startswith('<?xml'):
                        xml_entries.append((number_id, xml_content))
                elif section.strip().isdigit():
                    # Handle case where XML might be on same line
                    parts = section.split('<?xml', 1)
                    if len(parts) == 2:
                        number_id = parts[0].strip()
                        xml_content = '<?xml' + parts[1]
                        xml_entries.append((number_id, xml_content))

    except FileNotFoundError:
        print(f"Error: File '{filename}' not found.")
        return [], []
    except Exception as e:
        print(f"Error reading file: {e}")
        return [], []

    return text_entries, xml_entries

def find_text_duplicates(text_entries: List[Tuple[str, str]]) -> Dict[Tuple[str, str], List[int]]:
    """
    Find duplicates in text entries where both text and link_id match.

    Returns:
        Dictionary mapping (text, link_id) to list of occurrence indices
    """
    entry_occurrences = defaultdict(list)

    for i, (text, link_id) in enumerate(text_entries):
        entry_occurrences[(text, link_id)].append(i)

    # Filter to only duplicates (more than one occurrence)
    duplicates = {key: indices for key, indices in entry_occurrences.items() if len(indices) > 1}

    return duplicates

def find_xml_duplicates(xml_entries: List[Tuple[str, str]]) -> Dict[str, List[str]]:
    """
    Find duplicates in XML entries where the XML content matches.

    Returns:
        Dictionary mapping xml_content to list of number_ids that have that content
    """
    xml_occurrences = defaultdict(list)

    for number_id, xml_content in xml_entries:
        xml_occurrences[xml_content].append(number_id)

    # Filter to only duplicates (more than one occurrence)
    duplicates = {xml_content: number_ids for xml_content, number_ids in xml_occurrences.items() if len(number_ids) > 1}

    return duplicates

def main():
    filename = input("Enter the filename to check for duplicates: ").strip()

    print(f"Parsing file: {filename}")
    text_entries, xml_entries = parse_file(filename)

    print(f"Found {len(text_entries)} text entries with LINK IDs")
    print(f"Found {len(xml_entries)} XML entries")
    print()

    # Find text duplicates
    text_duplicates = find_text_duplicates(text_entries)

    if text_duplicates:
        print("=== TEXT + LINK DUPLICATES ===")
        for (text, link_id), indices in text_duplicates.items():
            print(f"Duplicate found:")
            print(f"  Text: {text}")
            print(f"  Link ID: {link_id}")
            print(f"  Occurrences: {len(indices)} times at positions {indices}")
            print()
    else:
        print("No text + link duplicates found.")

    # Find XML duplicates
    xml_duplicates = find_xml_duplicates(xml_entries)

    if xml_duplicates:
        print("=== XML CONTENT DUPLICATES ===")
        for i, (xml_content, number_ids) in enumerate(xml_duplicates.items()):
            print(f"Duplicate XML content #{i+1}:")
            print(f"  Found in entries: {', '.join(number_ids)}")
            print(f"  XML preview: {xml_content[:100]}{'...' if len(xml_content) > 100 else ''}")
            print()
    else:
        print("No XML content duplicates found.")

    # Summary
    total_duplicates = len(text_duplicates) + len(xml_duplicates)
    print(f"=== SUMMARY ===")
    print(f"Text + Link duplicates: {len(text_duplicates)}")
    print(f"XML content duplicates: {len(xml_duplicates)}")
    print(f"Total duplicate groups found: {total_duplicates}")

if __name__ == "__main__":
    main()