from typing import Any, Callable, Iterable

from exceptions import StreamInvalidOperationError
from optypes import OpType
from pipeline import Pipeline


class Stream:

    spliterator: Iterable[Any]
    head: "Pipeline" = Pipeline()

    def __init__(self) -> None:
        raise NotImplementedError()

    def of(self, args: Iterable[Any]) -> "Stream":
        if self.spliterator:
            raise StreamInvalidOperationError("The current Stream is inited")
        self.spliterator = args
    
    def map(self, op: Callable[[Any], Any]) -> "Stream": ## TODO: partial func optimization
        Pipeline.append(self.head, OpType.MAP, op)
