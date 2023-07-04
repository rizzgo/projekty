#
# @author xseipe
#

from storage.factory import StorageFactory


class BaseResource:
    def __init__(self, storage_factory: StorageFactory):
        self.storage_factory = storage_factory