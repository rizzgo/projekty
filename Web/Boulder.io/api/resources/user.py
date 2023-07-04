#
# @author xseipe
#

import json

import falcon
from resources.base_resource import BaseResource

class User(BaseResource):

    def on_post(self, req, resp):
        username: str = req.media["username"]
        if (self.storage_factory.user.get_by_username(username=username)):
            raise falcon.HTTPConflict
            
        self.storage_factory.user.store(user=req.media)
        doc = {"users": self.storage_factory.user.users}

        resp.text = json.dumps(doc, ensure_ascii=False)

    def on_put(self, req, resp):
        if req.auth:
            user = self.storage_factory.user.update(username=req.auth, body=req.media)
            resp.text = json.dumps(user, ensure_ascii=False)
        else:
            raise falcon.HTTPUnauthorized(description="you do not have profile")

    def on_get(self, req, resp):
        if req.auth:
            doc = self.storage_factory.user.get_by_username(username=req.auth)
            if not doc:
                raise falcon.HTTPNotFound(description="user not found")
            
            resp.text = json.dumps(doc, ensure_ascii=False)
        else:
            raise falcon.HTTPUnauthorized(description="you do not have profile")

    def on_get_by_username(self, req, resp, username):
        doc = self.storage_factory.user.get_by_username(username=username)
        if not doc:
            raise falcon.HTTPNotFound(description="user not found")
        
        resp.text = json.dumps(doc, ensure_ascii=False)
