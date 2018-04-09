/**
 * Copyright (c) 2018-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
package com.facebook.react.modules.websocket;

import com.facebook.common.logging.FLog;
import com.facebook.react.bridge.ReadableMap;
import com.facebook.react.bridge.ReadableMapKeySetIterator;
import com.facebook.react.bridge.ReadableType;
import com.facebook.react.common.ReactConstants;

import java.util.HashMap;
import java.util.Map;

public class WebSocketOptions {
  private static final String HEADERS_KEY = "headers";
  private static final String PING_INTERVAL_KEY = "pingInterval";

  private long mPingInterval;
  private final Map<String, String> mHeaders;

  public WebSocketOptions(ReadableMap options) {
    mPingInterval = 0;
    mHeaders = new HashMap<>();

    if (options != null) {
      mPingInterval = options.hasKey(PING_INTERVAL_KEY) ? (long) options.getDouble(PING_INTERVAL_KEY) : 0;

      if (options.hasKey(HEADERS_KEY)) {
        ReadableMap headers = options.getMap(HEADERS_KEY);

        ReadableMapKeySetIterator iterator = headers.keySetIterator();
        while (iterator.hasNextKey()) {
          String key = iterator.nextKey();
          if (ReadableType.String.equals(options.getType(key))) {
            mHeaders.put(key, options.getString(key));
          } else {
            FLog.w(
                    ReactConstants.TAG,
                    "Ignoring: requested " + key + ", value not a string");
          }
        }
      }
    }
  }

  public Map<String, String> getHeaders() {
    return mHeaders;
  }

  public long getPingInterval() {
    return mPingInterval;
  }
}
