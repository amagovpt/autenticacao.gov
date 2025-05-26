#!/usr/bin/env python3
import sys
sys.path.insert(1, "../GeneratedModule")
import pteid

def get_can():
    """Get Card Access Number from user input"""
    return input("Please enter CAN (Card Access Number): ")

def test_pteid_wrapper():
    """Test PTEID wrapper including PACE authentication"""
    try:
        print("Testing PTEID wrapper...")
        
        # Initialize the SDK
        print("Initializing SDK...")
        pteid.PTEID_ReaderSet.initSDK()

        # Get reader instance
        print("Getting reader set instance...")
        readers = pteid.PTEID_ReaderSet.instance()

        # Get number of readers
        reader_count = readers.readerCount()
        print(f"Number of connected readers: {reader_count}")

        # List all readers and find one with a card
        if reader_count > 0:
            print("\nLooking for readers with cards...")
            for i in range(reader_count):
                reader_name = readers.getReaderName(i)
                reader = readers.getReaderByName(reader_name)
                print(f"Reader {i + 1}: {reader_name}")
                
                if reader.isCardPresent():
                    print(f"Card found in reader: {reader_name}")
                    token = reader.getMultiPassToken()
                    print("Token: ", token.GetBytes())
        else:
            print("No readers found")

        print("\nPTEID wrapper test completed successfully!")
        return True

    except Exception as e:
        print(f"Error testing PTEID wrapper: {str(e)}")
        print(f"Error type: {type(e)}")
        return False

    finally:
        # Always release SDK
        print("\nReleasing SDK...")
        try:
            pteid.PTEID_ReaderSet.releaseSDK()
        except Exception as e:
            print(f"Error releasing SDK: {str(e)}")

if __name__ == "__main__":
    success = test_pteid_wrapper()
    sys.exit(0 if success else 1)
