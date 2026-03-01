#ifndef WISHLIST_LOGIC_H
#define WISHLIST_LOGIC_H

namespace wishlist::logic {

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

}  // namespace wishlist::logic

#endif