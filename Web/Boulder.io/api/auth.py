#
# @author xseipe
#


class Authentication:
    def process_request(self, req):
        if not req.auth:
            req.context.auth = None
        else:
            req.context.auth = req.auth