#
# @author xseipe
#

from storage.post import PostStorage
from storage.user import UserStorage

class StorageFactory:
    def __init__(self):
        self.post = PostStorage()
        self.user = UserStorage()