from typing import Any, Callable, Iterable
from functools import partial

from optypes import OpType
from pipeline import Pipeline


class Stream:

    spliterator: Iterable[Any]
    head: "Pipeline" = Pipeline(OpType.skip, None)

    @staticmethod
    def of(args: Iterable[Any]) -> "Stream":
        s = Stream()
        s.spliterator = args
        return s

    def call_method(self, op_name: str, *args) -> Any:
        op_type = OpType.name_of(op_name)
        Pipeline.append(self.head, op_type, args[0])
        if OpType.is_terminal(op_type):
            return Pipeline.execute(self.head, self.spliterator)
        return self
    
    def __getattr__(self, op_name: str) -> Any:
        return partial(self.call_method, op_name)
