//
// Created by David Hedbor on 2017-07-16.
//

import Foundation
#if os(Linux)
import Glibc
#else
import Darwin
#endif
struct System {
    var elw: UInt8 = 0
    var ww: UInt8 = 0
    var wwt: UInt8 = 0
    var tf: UInt8 = 0
    var aw: UInt8 = 0
    var value: UInt32 = 0
}

func synced(_ lock: NSLock, closure: () -> ()) {
    lock.lock()
    defer { lock.unlock() }
    closure()
}

extension Int  {
    static func seq(_ seq: Data.SubSequence) -> Int? {
        let len = seq.endIndex - seq.startIndex
        let bytesCopy = UnsafeMutablePointer<UInt8>.allocate(capacity: len + 1)
        seq.copyBytes(to: bytesCopy, from: seq.startIndex..<seq.endIndex)
        bytesCopy[len] = 0
        var value: Int?
        bytesCopy.withMemoryRebound(to: Int8.self, capacity: len+1) {
            value = atol($0)
        }
        return value;
    }
}

extension Double  {
    static func seq(_ seq: Data.SubSequence) -> Double {
        let len = seq.endIndex - seq.startIndex
        let bytesCopy = UnsafeMutablePointer<UInt8>.allocate(capacity: len + 1)
        seq.copyBytes(to: bytesCopy, from: seq.startIndex..<seq.endIndex)
        bytesCopy[len] = 0
        var value: Double = 0.0
        bytesCopy.withMemoryRebound(to: Int8.self, capacity: len+1) {
            value = atof($0)
        }
        return value
    }
}

extension String {
    init?(seq: Data.SubSequence) {
        var start = seq.startIndex
        var end = seq.endIndex

        let len = end - start
        if len > 2 {
            start += 1
            end -= 1
        }
        let data = seq.subdata(in: start..<end)
        self.init(data: data, encoding: .utf8)
    }
}
