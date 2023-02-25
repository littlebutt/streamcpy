class StreamError(Exception):
    pass

class StreamInvalidOperationError(StreamError):
    pass

class StreamCannotFindOpTypeError(StreamError):
    pass