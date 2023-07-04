#
# @author xseipe
#


class UserStorage():
    def __init__(self):
        self.users = []
        self.counter = 0

    def store(self, user):
        user["id"] = self.counter
        self.counter += 1
        self.users.append(user)

    def update(self, username, body):
        for user in self.users:
            if user["username"] == username:
                for key, value in body.items():
                    user[key] = value
                return user

    def get_by_username(self, username):
        for user in self.users:
            if user["username"] == username:
                return user

    def get_by_id(self, id):
        for user in self.users:
            if user["id"] == id:
                return user