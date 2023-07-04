#
# @author xseipe
#

import json

import falcon
from resources.base_resource import BaseResource

class Login(BaseResource):

    def on_post(self, req, resp):
        user = self.storage_factory.user.get_by_username(username=req.media["username"])
        if not user or (user["password"] != req.media["password"]):
            raise falcon.HTTPUnauthorized("invalid username or password")


        resp.text = json.dumps({}, ensure_ascii=False)
