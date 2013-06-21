#pragma once

namespace dbi {

/// Each query has a type. Need to specify the result.
enum QueryType {
   kSelect, kInsert, kCreate
};

}
