#ifndef WISHLIST_LOGIC_H
#define WISHLIST_LOGIC_H

namespace wishlist::logic {

enum class PurchaseOutcome {
  purchased,
  already_purchased,
  insufficient_funds,
  invalid_price,
};

struct PurchaseAttempt {
  PurchaseOutcome outcome;
  int cash_after;
  int popularity_after;
  bool purchased_after;
  bool should_close_menu;
  bool should_start_cursor_shake;
};

template <typename PurchasedFlags>
int first_unpurchased_index(const PurchasedFlags& purchased) {
  for (int i = 0; i < int(purchased.size()); ++i) {
    if (!purchased[i]) {
      return i;
    }
  }

  return 0;
}

inline int selected_price(bool is_purchased, int base_price) {
  return is_purchased ? 0 : base_price;
}

inline bool can_purchase(bool is_purchased, int base_price, int cash) {
  const int price = selected_price(is_purchased, base_price);
  return price > 0 && price <= cash;
}

inline PurchaseAttempt attempt_purchase(bool is_purchased, int base_price,
                                        int cash, int popularity_level) {
  if (is_purchased) {
    return {PurchaseOutcome::already_purchased,
            cash,
            popularity_level,
            true,
            false,
            false};
  }

  if (base_price <= 0) {
    return {PurchaseOutcome::invalid_price,
            cash,
            popularity_level,
            false,
            false,
            false};
  }

  if (cash < base_price) {
    return {PurchaseOutcome::insufficient_funds,
            cash,
            popularity_level,
            false,
            false,
            true};
  }

  return {PurchaseOutcome::purchased,
          cash - base_price,
          popularity_level + 1,
          true,
          true,
          false};
}

}  // namespace wishlist::logic

#endif