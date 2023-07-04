#
# @author xseipe
#

import json

import falcon
from resources.base_resource import BaseResource

class Post(BaseResource):

    def on_post(self, req, resp):
        self.storage_factory.post.store(post=req.media)
        resp.status = falcon.HTTP_200

    def on_post_vote(self, req, resp):
        if req.auth:
            post = self.storage_factory.post.store_vote(username=req.auth, body=req.media)
            resp.text = json.dumps(post, ensure_ascii=False)
        else:
            raise falcon.HTTPForbidden(description="you cannot vote")

    def on_post_comment(self,req,resp):
        if req.auth:
            post = self.storage_factory.post.store_comment(body=req.media)
            resp.text = json.dumps(post, ensure_ascii=False)
        else:
            raise falcon.HTTPForbidden(description="you cannot comment")   
     
    def on_get(self, req, resp):
        if req.params:
            doc = self.storage_factory.post.get_filtered(filters=req.params)
        else:
            doc = self.storage_factory.post.get_all()

        resp.text = json.dumps(doc, ensure_ascii=False)
        resp.status = falcon.HTTP_200

    def on_get_filtered(self, req, resp):
        doc = self.storage_factory.post.get_filtered(filters=req.params)
        
        resp.text = json.dumps(doc, ensure_ascii=False)
        resp.status = falcon.HTTP_200

    def on_get_by_username(self, req, resp, username):
        if req.params:
            doc = self.storage_factory.post.get_by_username_filtered(username=username, filters=req.params)
        else:
            doc = self.storage_factory.post.get_by_username(username=username)
        
        resp.text = json.dumps(doc, ensure_ascii=False)
        resp.status = falcon.HTTP_200

