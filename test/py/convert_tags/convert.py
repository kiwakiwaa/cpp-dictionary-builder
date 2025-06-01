import os
import json
from bs4 import BeautifulSoup
import glob
from pprint import pprint

def load_tag_map(json_file):
    """Load the tag mapping from JSON file."""
    try:
        with open(json_file, 'r', encoding='utf-8') as f:
            return json.load(f)
    except FileNotFoundError:
        print(f"Error: {json_file} not found!")
        return None
    except json.JSONDecodeError:
        print(f"Error: Invalid JSON in {json_file}")
        return None

def replace_class_names(class_attr, class_map):
    """Replace class names in a class attribute string."""
    if not class_attr:
        return class_attr

    # Split classes by space
    classes = class_attr.split()
    replaced_classes = []

    for cls in classes:
        # Replace if mapping exists, otherwise keep original
        replaced_classes.append(class_map.get(cls, cls))

    return ' '.join(replaced_classes)

def analyze_changes(soup, tag_map):
    """Analyze and print changes that would be made without modifying the file."""
    changes = {
        'tags': {},
        'classes': {}
    }

    tag_mapping = tag_map.get('tag', {})
    class_mapping = tag_map.get('class', {})

    # Find tag changes
    for old_tag, new_tag in tag_mapping.items():
        elements = soup.find_all(old_tag)
        if elements:
            changes['tags'][old_tag] = {
                'new_name': new_tag,
                'count': len(elements)
            }

    # Find class changes
    for element in soup.find_all(attrs={'class': True}):
        original_class = element.get('class')
        if isinstance(original_class, list):
            class_string = ' '.join(original_class)
        else:
            class_string = original_class

        new_class_string = replace_class_names(class_string, class_mapping)

        if class_string != new_class_string:
            changes['classes'][class_string] = {
                'new_name': new_class_string,
                'element': element.name
            }

    return changes

def process_xml_file(file_path, tag_map, test_mode=False):
    """Process a single XML file and replace tags and classes."""
    try:
        # Read the XML file
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()

        # Parse with BeautifulSoup
        soup = BeautifulSoup(content, 'xml')

        if test_mode:
            print(f"\n=== TEST MODE: Analyzing {os.path.basename(file_path)} ===")
            changes = analyze_changes(soup, tag_map)

            if changes['tags']:
                print("\nTag changes:")
                pprint(changes['tags'])
            else:
                print("\nNo tag changes needed")

            if changes['classes']:
                print("\nClass changes:")
                pprint(changes['classes'])
            else:
                print("\nNo class changes needed")

            print("\n=== END TEST ===")
            return True

        tag_mapping = tag_map.get('tag', {})
        class_mapping = tag_map.get('class', {})
        specific_mapping = tag_map.get('specific', {})

        # Replace tag names
        for old_tag, new_tag in tag_mapping.items():
            # Find all elements with the old tag name
            elements = soup.find_all(old_tag)
            for element in elements:
                # If new tag is "table", add original tag name as class
                if new_tag.lower() in ["table", "td", "tr"]:
                    # Get existing classes
                    existing_classes = element.get('class', [])
                    if isinstance(existing_classes, str):
                        existing_classes = existing_classes.split()
                    elif existing_classes is None:
                        existing_classes = []

                    #if old_tag == "ImageG" and "Sashie" not in existing_classes:
                        #continue

                    # Add original tag name as class if not already present
                    if old_tag not in existing_classes:
                        existing_classes.append(old_tag)

                    element['class'] = existing_classes

                element.name = new_tag

        # Replace class names
        for element in soup.find_all(attrs={'class': True}):
            original_class = element.get('class')

            if isinstance(original_class, list):
                class_string = ' '.join(original_class)
            else:
                class_string = original_class

            new_class_string = replace_class_names(class_string, class_mapping)
            element['class'] = new_class_string.split()

        # Write back to file
        if not test_mode:
            with open(file_path, 'w', encoding='utf-8') as f:
                f.write(str(soup))
            print(f"Processed: {os.path.basename(file_path)}")

        return True

    except Exception as e:
        print(f"Error processing {file_path}: {str(e)}")
        return False

def main():
    """Main function to process all XML files in a user-specified directory."""
    # Prompt user for directory path
    folder_path = input("Enter the path to the folder containing XML files: ").strip()

    # Check if the path exists and is a directory
    if not os.path.isdir(folder_path):
        print("Invalid folder path. Please make sure the folder exists.")
        return

    # Load tag mapping (assumes tag_map.json is still next to this script)
    script_dir = os.path.dirname(os.path.abspath(__file__))
    tag_map_file = os.path.join(script_dir, 'tag_map.json')
    tag_map = load_tag_map(tag_map_file)

    if tag_map is None:
        return

    # Find all XML files in the provided directory
    xml_pattern = os.path.join(folder_path, '*.xml')
    xml_files = glob.glob(xml_pattern)

    if not xml_files:
        print("No XML files found in the specified directory.")
        return

    print(f"Found {len(xml_files)} XML files to process.")

    # Ask user if they want to test first
    test_mode = input("Run in test mode first? (y/n): ").strip().lower() == 'y'

    if test_mode:
        print("\nRunning in TEST MODE - no files will be modified")
        test_file = input("Enter filename to test (or press Enter for first file): ").strip()

        if not test_file:
            test_file = xml_files[0]
        elif not os.path.exists(test_file):
            print(f"File {test_file} not found!")
            return

        process_xml_file(test_file, tag_map, test_mode=True)

        proceed = input("\nProceed with actual changes? (y/n): ").strip().lower() == 'y'
        if not proceed:
            return

    # Process each XML file
    successful = 0
    for xml_file in xml_files:
        if process_xml_file(xml_file, tag_map):
            successful += 1

    print(f"\nProcessing complete! Successfully processed {successful}/{len(xml_files)} files.")

if __name__ == "__main__":
    main()