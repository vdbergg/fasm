#include <bitset>
#include <iostream>
#include <string>
#include <climits>

using namespace std;

unsigned ALPHABET_SIZE = 128;


inline void printBinaryAndHexFromDecimal(unsigned int decimal, unsigned bits_size) {
  if (bits_size == 32) {
    bitset<sizeof(unsigned int) * CHAR_BIT> binary(decimal); //sizeof() returns bytes, not bits!
//    cout << "D: " << decimal << "\t";
//    cout << "H: 0x" << hex << (unsigned long) decimal << "\t";
    cout << "B: " << binary << endl;
  } else if (bits_size == 64) {
    bitset<sizeof(unsigned long) * CHAR_BIT> binary(decimal); //sizeof() returns bytes, not bits!
//    cout << "D: " << decimal << "\t";
//    cout << "H: 0x" << hex << (unsigned long) decimal << "\t";
    cout << "B: " << binary << endl;
  }
}


unsigned long s_j(unsigned long x, unsigned int i, int j) {
  return (x >> i) & ((1 << j) - 1);
}


void fasm(string &text, string &pattern, int editDistanceThreshold) {
  int textSize = text.size();
  int patternSize = pattern.size();
  unsigned long maskFromPatternSize[ALPHABET_SIZE];
  unsigned long mask[ALPHABET_SIZE];
  unsigned long S[ALPHABET_SIZE];

  unsigned long initialValue = (1 << patternSize) - 1;
  string subPattern = pattern.substr(0, editDistanceThreshold + 1);
  cout << subPattern << endl;

  // Pre processing
  for (unsigned i = 0; i < ALPHABET_SIZE; i++) {
    maskFromPatternSize[i] = initialValue;

    for (int j = patternSize - 1; j >= 0; j--) {
      if (pattern[j] == i) {
        maskFromPatternSize[i] &= (~ (1 << j));
      }
    }

    for (unsigned j = 0; j <= patternSize - editDistanceThreshold - 1; j++) {
      if (j == 0) {
        mask[i] = s_j(maskFromPatternSize[i], j, editDistanceThreshold + 1);
      } else {
        mask[i] = (mask[i] << (editDistanceThreshold + 2)) | s_j(maskFromPatternSize[i], j, editDistanceThreshold + 1);
      }
    }

    S[i] = subPattern.find(i) != string::npos ? 1 : 0;
  }

  unsigned long D_in_mask = (1 << (editDistanceThreshold + 1)) - 1;
  unsigned long M1_mask = 1;

  unsigned long D_in = D_in_mask;
  for (unsigned i = 0; i < patternSize - editDistanceThreshold; i++) {
    D_in = (D_in << (editDistanceThreshold + 2)) | D_in_mask;
  }

  unsigned long M1 = M1_mask;
  for (unsigned i = 0; i < patternSize - editDistanceThreshold; i++) {
    M1 = (M1 << (editDistanceThreshold + 2)) | M1_mask;
  }

  unsigned long M2 = M1_mask;
  for (unsigned i = 0; i < patternSize - editDistanceThreshold - 1; i++) {
    M2 = (M2 << (editDistanceThreshold + 2)) | M1_mask;
  }
  M2 = (M2 << (editDistanceThreshold + 2)) | D_in_mask;

  unsigned long M3 = D_in_mask;

  unsigned long G = 1 << editDistanceThreshold;

  unsigned long D = D_in;
  int i = 0;

  while (i < textSize) {
    if (S[text[i]]) {
      do {
        unsigned long x = (D >> (editDistanceThreshold + 2)) | mask[text[i]];
        D = ((D << 1) | M1) & ((D << (editDistanceThreshold + 3)) | M2) & (((x + M1) ^ x) >> 1) & D_in;

        if ((D & G) == 0) {
          cout << "Match ending at: " << i << endl;
          D |= M3;
        }

        i++;
      } while ((D != D_in) && (i <= textSize));
    }
    i++;
  }
}

int main() {
  string text = "joao e maria juntos em test e text";
  string pattern = "text";

  for (int i = 0; i < text.size(); i++) {
    cout << "i: " << i << " char: " << text[i] << endl;
  }

  fasm(text, pattern, 0);
  return 0;
}
