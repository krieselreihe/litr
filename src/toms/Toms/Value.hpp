#pragma once

#include <toml.hpp>
#include <tsl/ordered_map.h>

#include <variant>
#include <string>
#include <vector>

namespace Litr::Toms {

template<typename V>
class ValueIterator {
 public:
  using ValueType = typename V::ValueType;
  using PointerType = ValueType*;
  using ReferenceType = ValueType&;

 public:
  explicit ValueIterator(PointerType ptr) : m_Ptr(ptr) {}

  // Prefix increment
  ValueIterator& operator++() {
    m_Ptr++;
    return *this;
  }

  // Postfix increment
  ValueIterator operator++(int) {
    ValueIterator iterator{*this};
    ++(*this);
    return iterator;
  }

  // Prefix decrement
  ValueIterator& operator--() {
    m_Ptr--;
    return *this;
  }

  // Postfix decrement
  ValueIterator operator--(int) {
    ValueIterator iterator{*this};
    --(*this);
    return iterator;
  }

  ReferenceType operator[](int index) {
    return *(m_Ptr + index);
  }

  PointerType operator->() {
    return m_Ptr;
  }

  ReferenceType operator*() {
    return *m_Ptr;
  }

  bool operator==(const ValueIterator& other) const {
    return m_Ptr == other.m_Ptr;
  }

  bool operator!=(const ValueIterator& other) const {
    return !(*this == other);
  }

 private:
  PointerType m_Ptr;
};

class Value {
 public:
  using T = toml::basic_value<toml::discard_comments, tsl::ordered_map>;
  using ValueType = T;
  using Iterator = ValueIterator<Value>;

  Value() = default;
  explicit Value(const std::string& path);
  explicit Value(const toml::value& data);

  [[nodiscard]] Value Find(const std::string& key) const;
  [[nodiscard]] bool Contains(const std::string& key) const;

  [[nodiscard]] bool IsTable() const;
  [[nodiscard]] bool IsString() const;
  [[nodiscard]] bool IsArray() const;

  [[nodiscard]] Value AsTable() const;
  [[nodiscard]] std::string AsString() const;
  [[nodiscard]] std::vector<Value> AsArray() const;

  [[nodiscard]] Value At(size_t key) const;
  [[nodiscard]] Value At(const std::string& key) const;

  [[nodiscard]] uint32_t GetLine() const { return m_Line; }
  [[nodiscard]] uint32_t GetColumn() const { return m_Column; }
  [[nodiscard]] std::string GetContent() const { return m_Content; }

  [[nodiscard]] bool HasError() const { return m_HasError; }
  [[nodiscard]] size_t Size() const { return m_Data->size(); }

  Value operator[](size_t index) const {
    return At(index);
  }

  Value operator[](size_t index) {
    return At(index);
  }

  Iterator begin() {
    return Iterator(m_Data);
  }

  Iterator end() {
    return Iterator(m_Data + Size());
  }

 private:
  bool m_HasError{false};

  T* m_Data{};
  uint32_t m_Line{};
  uint32_t m_Column{};
  std::string m_Content{};
};

}  // namespace Litr::Toms
