#include <vector>
#include <map>
#include <algorithm>

template<typename Key, typename T, typename Compare = std::less<Key>>
class ContiguousMap
{
public:
  using Base = typename std::vector<std::pair<Key, T>>;
  using iterator = typename Base::iterator;
  using const_iterator = typename Base::const_iterator;
  using reverse_iterator = typename Base::reverse_iterator;
  using const_reverse_iterator = typename Base::const_reverse_iterator;
  using reference = typename Base::reference;
  using const_reference = typename Base::const_reference;
  using pointer = typename Base::pointer;
  using const_pointer = typename Base::const_pointer;
  using value_type = std::pair<Key, T>;

public:
  static bool key_compare(const value_type& x, const value_type& y)
  {
    Compare comp;
    return comp(x.first, y.first);
  }

private:
  struct KeyComp
  {
    template<typename T1, typename T2>
    bool operator()(const T1& k1, const T2& k2) const
    {
      if constexpr(std::is_same_v<T1, Key> && std::is_same_v<T2, Key>)
      {
        return k1 < k2;
      }
      else if constexpr(std::is_same_v<T1, Key> &&
                        std::is_same_v<T2, value_type>)
      {
        return k1 < k2.first;
      }
      else if constexpr(std::is_same_v<T1, value_type> &&
                        std::is_same_v<T2, Key>)
      {
        return k1.first < k2;
      }
      else
      {
        return k1.first < k2.first;
      }
    }
  };

public:
  ContiguousMap() = default;

  template<typename InputIt>
  ContiguousMap(InputIt b, InputIt e)
  {
    std::map<Key, T, Compare> filter_duplicates(b, e);
    elements_.reserve(filter_duplicates.size());
    std::copy(filter_duplicates.cbegin(), filter_duplicates.cend(),
              std::back_inserter(elements_));
  }

  ContiguousMap(const std::initializer_list<value_type>& l) :
      ContiguousMap(l.begin(), l.end())
  {
  }

  iterator begin()
  {
    return elements_.begin();
  }

  const_iterator begin() const
  {
    return elements_.cbegin();
  }

  iterator end()
  {
    return elements_.end();
  }

  const_iterator end() const
  {
    return elements_.cend();
  }

  const_iterator cbegin() const
  {
    return elements_.cbegin();
  }

  const_iterator cend() const
  {
    return elements_.cend();
  }

  iterator lower_bound(const Key& k)
  {
    return std::lower_bound(elements_.begin(), elements_.end(), k, KeyComp());
  }

  const_iterator lower_bound(const Key& k) const
  {
    return std::lower_bound(elements_.cbegin(), elements_.cend(), k, KeyComp());
  }

  iterator upper_bound(const Key& k)
  {
    return std::upper_bound(elements_.begin(), elements_.end(), k, KeyComp());
  }

  const_iterator upper_bound(const Key& k) const
  {
    return std::upper_bound(elements_.cbegin(), elements_.cend(), k, KeyComp());
  }

  size_t size() const
  {
    return elements_.size();
  }

  void clear()
  {
    elements_.clear();
  }

  std::pair<iterator, bool> insert(const value_type& value)
  {
    auto it = lower_bound(value.first);
    bool not_found = false;
    if(it == elements_.cend() || key_compare(value, *it))
    {
      elements_.insert(it, value);
      not_found = true;
    }
    return std::make_pair(it, not_found);
  }

  template<typename InputIt>
  void insert(InputIt first, InputIt last)
  {
    while(first != last)
    {
      insert(*first);
      ++first;
    }
  }

  void erase(iterator pos)
  {
    elements_.erase(pos);
  }

  void erase(const_iterator pos)
  {
    elements_.erase(pos);
  }

  bool erase(const Key& k)
  {
    auto it = find(k);
    if(it == elements_.end())
    {
      return false;
    }
    else
    {
      elements_.erase(it);
      return true;
    }
  }

  iterator find(const Key& k)
  {
    auto it = lower_bound(k);
    if(it != elements_.end() && KeyComp()(k, it->first))
    {
      it = elements_.end();
    }
    return it;
  }

  const_iterator find(const Key& k) const
  {
    auto it = lower_bound(k);
    if(it != elements_.cend() && KeyComp()(k, it->first))
    {
      it = elements_.cend();
    }
    return it;
  }

  bool contains(const Key& k) const
  {
    return std::binary_search(elements_.cbegin(), elements_.cend(), k,
                              KeyComp());
  }

  auto operator<=>(const ContiguousMap& rhs) const = default;

  void swap(ContiguousMap& rhs)
  {
    std::swap(elements_, rhs.elements_);
  }

private:
  std::vector<std::pair<Key, T>> elements_;
};
