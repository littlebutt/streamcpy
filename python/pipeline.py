from typing import Callable, Iterable, Optional, Any, Union
from functools import reduce

from optypes import OpType


class Pipeline:
    op_type: OpType
    op_method: Union[int, Callable]
    next: Optional["Pipeline"] = None

    def __init__(self, op_type: OpType, op_method: Callable) -> None:
        self.op_type = op_type
        self.op_method = op_method

    @staticmethod
    def append(pl: "Pipeline", op_type: OpType, op_method: Callable) -> None:
        ptr = pl
        while ptr.next:
            ptr = ptr.next
        ptr.next = Pipeline(op_type=op_type, op_method=op_method)

    @staticmethod
    def execute(pl: "Pipeline", init_data: Iterable[Any]) -> Any:
        ptr = pl
        data = init_data
        while ptr:
            if ptr.op_type == OpType.skip:
                ptr = ptr.next
                continue
            if OpType.execute_with_element(ptr.op_type):
                if OpType.filter == ptr.op_type:
                    data = [x for x in filter(ptr.op_method, data)]
                else:
                    data = [ptr.op_method(x) for x in data]
            else:
                if OpType.collect == ptr.op_type:
                    return data
                if OpType.reduce == ptr.op_type:
                    return reduce(ptr.op_method, data)
                if OpType.distinct == ptr.op_type:
                    _memo = set()
                    data = [x for x in data if x not in _memo and not _memo.add(x)]
                elif OpType.limit == ptr.op_type:
                    data = data[:ptr.op_method]
                else:
                    data = ptr.op_method(data)
            ptr = ptr.next