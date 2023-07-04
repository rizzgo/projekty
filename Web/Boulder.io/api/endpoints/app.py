#
# @author xseipe00
#

import falcon
from resources.post import Post
from resources.user import User
from resources.login import Login
from storage.factory import StorageFactory         

app = application = falcon.App(cors_enable=True)

storage_factory = StorageFactory()

app.add_route('/posts', Post(storage_factory=storage_factory))
app.add_route('/posts/{username}', Post(storage_factory=storage_factory), suffix="by_username")
app.add_route('/posts/vote', Post(storage_factory=storage_factory), suffix="vote")
app.add_route('/posts/comment', Post(storage_factory=storage_factory), suffix="comment")
app.add_route('/posts/filtered', Post(storage_factory=storage_factory), suffix="filtered")
app.add_route('/users', User(storage_factory=storage_factory))
app.add_route('/users/{username}', User(storage_factory=storage_factory), suffix="by_username")
app.add_route('/login', Login(storage_factory=storage_factory))
