from typing import Callable, Iterable, Optional, Any

from optypes import OpType


class Pipeline:
    op_type: OpType
    op_method: Optional[Callable]
    next: Optional["Pipeline"] = None

    def __init__(self) -> None:
        pass

    def __init__(self, op_type: OpType, op_method: Callable) -> None:
        self.op_type = op_type
        self.op_method = op_method

    @classmethod
    def append(cls, pl: "Pipeline", op_type: OpType, op_method: Callable) -> None:
        ptr = pl
        while ptr.next:
            ptr = ptr.next
        ptr.next = Pipeline(op_type=op_type, op_method=op_method)

    @classmethod
    def execute(cls, pl: "Pipeline", init_data: Iterable[Any]) -> Any:
        raise NotImplementedError()