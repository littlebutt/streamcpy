from enum import Enum

from exceptions import StreamCannotFindOpTypeError

class OpType(Enum):
    skip = 0,
    map = 1,
    filter = 2,
    for_each = 3,
    collect = 4

    def name_of(op_type_name: str) -> "OpType":
        for k, v in OpType._member_map_:
            if op_type_name == k:
                return v
        raise StreamCannotFindOpTypeError(f"OpType {op_type_name} is not defined")

