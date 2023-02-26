from typing import Callable, Iterable, Optional, Any

from optypes import OpType


class Pipeline:
    op_type: OpType
    op_method: Optional[Callable]
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
                data = [ptr.op_method(x) for x in data]
            else:
                data = ptr.op_method(data)
            ptr = ptr.next