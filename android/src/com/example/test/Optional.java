package com.example.test;

import com.example.test.Optional;

public abstract class Optional<K> {
  abstract K get();
  abstract boolean isPresent();

  public static <K> Optional<K> of(final K value) {
    return new Optional<K>() {
      @Override
      public K get() {
        return value;
      }

      @Override
      public boolean isPresent() {
        return true;
      }
    };
  }

  public static <K> Optional<K> absent() {
    return new Optional<K>() {
      @Override
      public K get() {
        return null;
      }

      @Override
      public boolean isPresent() {
        return false;
      }
    };
  }
}

