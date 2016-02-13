#include <bandit/bandit.h>
#include <string>
#include <vector>
#include <map>
#include <array>
#include <iostream>
#include <stdexcept>

using namespace bandit;
using namespace std;

using relation = vector<pair<string, int>>;

using relations = array<relation, 4>;

relations conversion_relations {
  relation {
    {"", 0},
    {"I", 1},
    {"II", 2},
    {"III", 3},
    {"IV", 4},
    {"V", 5},
    {"VI", 6},
    {"VII", 7},
    {"VIII", 8},
    {"IX", 9}
  },
  relation {
    {"", 0},
    {"X", 10},
    {"XX", 20},
    {"XXX", 30},
    {"XL", 40},
    {"L", 50},
    {"LX", 60},
    {"LXX", 70},
    {"LXXX", 80},
    {"XC", 90}
  },
  relation {
    {"", 0},
    {"C", 100},
    {"CC", 200},
    {"CCC", 300},
    {"CD", 400},
    {"D", 500},
    {"DC", 600},
    {"DCC", 700},
    {"DCCC", 800},
    {"CM", 900}
  },
  relation {
    {"", 0},
    {"M", 1000},
    {"MM", 2000},
    {"MMM", 3000}
  }
};

using combination = array<relation::iterator, 4>;

combination next(combination c)
{
  // FIXME: those nested ifs were intentional
  // The idea is refactor them and see the before/after afterwards :-)
  if (next(c[0]) == end(conversion_relations[0])) {
    c[0] = begin(conversion_relations[0]);
    if (next(c[1]) == end(conversion_relations[1])) {
      c[1] = begin(conversion_relations[1]);
      if (next(c[2]) == end(conversion_relations[2])) {
        c[2] = begin(conversion_relations[2]);
        if (next(c[3]) == end(conversion_relations[3])) {
          c[3] = begin(conversion_relations[3]);
        } else {
          c[3] = next(c[3]);
        }
      } else {
        c[2] = next(c[2]);
      }
    } else {
      c[1] = next(c[1]);
    }
  } else {
    c[0] = next(c[0]);
  }

  return c;
}

const combination combination_begin {
  begin(conversion_relations[0]),
  begin(conversion_relations[1]),
  begin(conversion_relations[2]),
  begin(conversion_relations[3])
};

const combination combination_end {
  end(conversion_relations[0]),
  end(conversion_relations[1]),
  end(conversion_relations[2]),
  end(conversion_relations[3])
};

combination nth_combination(int nth)
{
  auto c = combination_begin;

  while (nth-- > 0) {
    c = next(c);
  }

  return c;
}

int combination_arabic(const combination &c)
{
  return std::accumulate(begin(c), end(c), 0, [](int cur, const auto &pair) {
    return cur + pair->second;
  });
}

string combination_roman(const combination &c)
{
  return std::accumulate(rbegin(c), rend(c), string(), [](const string &cur, const auto &pair) {
    return cur + pair->first;
  });
}

int roman_to_arabic(const string &roman)
{
  for (auto c = combination_begin; c != combination_end; c = next(c)) {
    if (combination_roman(c) == roman) {
      return combination_arabic(c);
    }
  }

  return -1;
}

string arabic_to_roman(int arabic)
{
  // FIXME: this has a terrible performance, but who cares?! :-)
  return combination_roman(nth_combination(arabic));
}

go_bandit([]{
  describe("Combinations order", [&]{
    it("ensures combination order", [&] {
      for (int i = 0; i < 3999; i++) {
        auto comb = nth_combination(i);
        AssertThat(combination_arabic(comb), Equals(i)); 
      }
    });
  });

  describe("Slooooow roman to arabic and vice-versa implementation", []{
    it("converts an empty string to 0", [&] {
      AssertThat(roman_to_arabic(""), Equals(0));
    });

    it("converts I empty string to 1", [&] {
      AssertThat(roman_to_arabic("I"), Equals(1));
    });

    it("converts 0 to empty string", [&] {
      AssertThat(arabic_to_roman(0), Equals(""));
    });

    it("validate all numbers, by converting back and forth", [&] {
      for (int i = 0; i < 3999; i++) {
        AssertThat(roman_to_arabic(arabic_to_roman(i)), Equals(i));
      }
    });
  });
});

int main(int argc, char **argv)
{
  return run(argc, argv);
}
