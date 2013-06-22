#pragma once

namespace dbi {

/// An operator can be open or closed. No logic depends on these values, but it prevents operators from being used incorrect.
enum OperatorState {
   kOpen, kClosed
};

}
