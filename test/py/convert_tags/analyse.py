import os
import glob
from bs4 import BeautifulSoup
from pathlib import Path

def parse_xml_files(folder_path):
    """
    Parse all XML files in a folder and extract unique tag names and class names.

    Args:
        folder_path (str): Path to the folder containing XML files

    Returns:
        tuple: (set of tag names, set of class names)
    """
    # Sets to store unique tag names and class names
    all_tag_names = set()
    all_class_names = set()

    # Get all XML files in the folder
    xml_pattern = os.path.join(folder_path, "*.xml")
    xml_files = glob.glob(xml_pattern)

    if not xml_files:
        print(f"No XML files found in {folder_path}")
        return all_tag_names, all_class_names

    print(f"Found {len(xml_files)} XML files to process...")

    # Process each XML file
    for i, xml_file in enumerate(xml_files, 1):
        try:
            print(f"Processing file {i}/{len(xml_files)}: {os.path.basename(xml_file)}")

            # Read and parse the XML file
            with open(xml_file, 'r', encoding='utf-8') as file:
                content = file.read()
                soup = BeautifulSoup(content, 'xml')

            # Extract all elements from the parsed XML
            all_elements = soup.find_all()

            # Collect tag names
            for element in all_elements:
                all_tag_names.add(element.name)

                # Collect class names if they exist
                if element.has_attr('class'):
                    # Classes can be a list or a string
                    classes = element['class']
                    if isinstance(classes, list):
                        # Join the list with spaces in case there are multiple classes in one item
                        classes = ' '.join(classes)

                    # Split by spaces to handle multiple class names
                    for class_name in classes.split():
                        all_class_names.add(class_name.strip())

        except Exception as e:
            print(f"Error processing {xml_file}: {str(e)}")
            continue

    return all_tag_names, all_class_names

def main():
    # Specify the folder path containing XML files
    folder_path = input("Enter the path to the folder containing XML files: ").strip()

    # Check if folder exists
    if not os.path.exists(folder_path):
        print(f"Error: Folder '{folder_path}' does not exist.")
        return

    if not os.path.isdir(folder_path):
        print(f"Error: '{folder_path}' is not a directory.")
        return

    print(f"\nScanning folder: {folder_path}")
    print("-" * 50)

    # Parse all XML files
    tag_names, class_names = parse_xml_files(folder_path)

    # Display results
    print("\n" + "="*50)
    print("PARSING COMPLETE")
    print("="*50)

    print(f"\nUnique Tag Names Found ({len(tag_names)} total):")
    print("-" * 30)
    if tag_names:
        for tag in sorted(tag_names):
            print(f"  - {tag}")
    else:
        print("  No tag names found.")

    print(f"\nUnique Class Names Found ({len(class_names)} total):")
    print("-" * 30)
    if class_names:
        for class_name in sorted(class_names):
            print(f"  - {class_name}")
    else:
        print("  No class names found.")

    # Optional: Save results to file
    save_to_file = input("\nWould you like to save results to a file? (y/n): ").strip().lower()
    if save_to_file == 'y':
        output_file = "xml_analysis_results.txt"
        with open(output_file, 'w', encoding='utf-8') as f:
            f.write("XML Analysis Results\n")
            f.write("="*50 + "\n\n")

            f.write(f"Unique Tag Names ({len(tag_names)} total):\n")
            f.write("-" * 30 + "\n")
            for tag in sorted(tag_names):
                f.write(f"  - {tag}\n")

            f.write(f"\nUnique Class Names ({len(class_names)} total):\n")
            f.write("-" * 30 + "\n")
            for class_name in sorted(class_names):
                f.write(f"  - {class_name}\n")

        print(f"Results saved to {output_file}")

if __name__ == "__main__":
    # Make sure BeautifulSoup is installed
    try:
        from bs4 import BeautifulSoup
    except ImportError:
        print("Error: BeautifulSoup4 is not installed.")
        print("Please install it using: pip install beautifulsoup4")
        exit(1)

    main()