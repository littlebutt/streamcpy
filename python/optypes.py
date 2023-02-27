from enum import Enum

from exceptions import StreamCannotFindOpTypeError


class OpType(Enum):
    skip = 0,
    map = 1,
    filter = 2,
    distinct = 3,
    limit = 4,
    for_each = 5,
    collect = 6

    @staticmethod
    def name_of(op_type_name: str) -> "OpType":
        for k, v in OpType._member_map_.items():
            if op_type_name == k:
                return v
        raise StreamCannotFindOpTypeError(f"OpType {op_type_name} is not defined")

    @staticmethod
    def is_terminal(op_type: "OpType") -> bool:
        if op_type in [OpType.for_each, OpType.collect]:
            return True
        else:
            return False

    @staticmethod
    def execute_with_element(op_type: "OpType") -> bool:
        if op_type in [OpType.map, OpType.filter, OpType.for_each]:
            return True
        else:
            return False
