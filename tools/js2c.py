import os
import re
import sys
import string

def main():
  natives = sys.argv[1]
  source_files = sys.argv[2]
  if source_files[-2] == '-t':
      global TEMPLATE
      TEMPLATE = source_files[-1]

if __name__ == "__main__":
    main()