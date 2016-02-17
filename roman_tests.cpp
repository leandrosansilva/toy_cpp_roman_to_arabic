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

using components = array<unsigned int, 4>;

constexpr components extract_components(unsigned int arabic)
{
  auto m = arabic / 1000u;
  auto c = arabic % 1000u;
  auto d = c % 100u;
  return {m, c / 100u, d / 10u, d % 10};
}

using combination = array<relation::iterator, 4>;

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

combination next(const combination &c)
{
  return get<2>(accumulate(begin(c), end(c), make_tuple(true, 0u, c), [](auto acc, auto cur) {
    auto has_overflow = get<0>(acc);
    auto index = get<1>(acc);
    auto c = get<2>(acc);
    auto next_index = index + 1;

    if (!has_overflow) {
      return make_tuple(false, next_index, c);
    }

    auto n = next(cur);

    if (n == end(conversion_relations[index])) {
      c[index] = begin(conversion_relations[index]);
      return make_tuple(true, next_index, c);
    }

    c[index] = n;
    return make_tuple(false, next_index, c);
  }));
}

combination nth_combination(unsigned int nth)
{
  auto c = combination_begin;

  while (nth-- > 0) {
    c = next(c);
  }

  return c;
}

int combination_arabic(const combination &c)
{
  return std::accumulate(begin(c), end(c), 0, [](unsigned int cur, const auto &pair) {
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

string arabic_to_roman(unsigned int arabic)
{
  auto comps = extract_components(arabic);

  combination comb;

  for (size_t i = 0, size = comps.size(); i < size; i++) {
    comb[size - 1 - i] = conversion_relations[size - 1 - i].begin() + comps[i];
  }

  return combination_roman(comb);
}

go_bandit([]{
  describe("Obtains the 'components' of a arabic number", []{
    it("obtains 0,0,0,0 from 0", [&] {
      AssertThat(extract_components(0), EqualsContainer(components{0, 0, 0, 0}));
    });

    it("obtains 0,0,0,4 from 4", [&] {
      AssertThat(extract_components(4), EqualsContainer(components{0, 0, 0, 4}));
    });

    it("obtains 0,0,3,5 from 35", [&] {
      AssertThat(extract_components(35), EqualsContainer(components{0, 0, 3, 5}));
    });

    it("obtains 2,5,7,3 from 2573", [&] {
      AssertThat(extract_components(2573), EqualsContainer(components{2, 5, 7, 3}));
    });
  });

  describe("Slooooow roman to arabic and vice-versa implementation", []{
    it("converts an empty string to 0", [&] {
      AssertThat(roman_to_arabic(""), Equals(0));
    });

    it("converts I empty string to 1", [&] {
      AssertThat(roman_to_arabic("I"), Equals(1));
    });

    it("converts XII empty string to 12", [&] {
      AssertThat(roman_to_arabic("XII"), Equals(12));
    });

    it("converts 0 to empty string", [&] {
      AssertThat(arabic_to_roman(0), Equals(""));
    });

    it("converts 245 to empty CCXLV", [&] {
      AssertThat(arabic_to_roman(245), Equals("CCXLV"));
    });

    it("validate all numbers, by converting back and forth", [&] {
      for (int i = 0; i <= 3999; i++) {
        AssertThat(roman_to_arabic(arabic_to_roman(i)), Equals(i));
      }
    });
  });
});

int main(int argc, char **argv)
{
  return run(argc, argv);
}
