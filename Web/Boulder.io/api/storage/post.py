#
# @author xseipe
#


class PostStorage():
    def __init__(self):
        self.posts = []
        self.counter = 0

    def __update_rating(self, post):
        up = 0
        down = 0
        for value in post["votes"].values():
            if value == "up":
                up += 1
            elif value == "down":
                down += 1
        post["rating"] = up - down

    def store(self, post):
        post["id"] = self.counter
        self.counter += 1
        self.posts.append(post)

    def store_vote(self, username, body):
        for post in self.posts:
            if post["id"] == body["id"]:
                if username not in post["votes"]:
                    post["votes"][username] = body["vote"]
                    self.__update_rating(post)
                    return post

    def store_comment(self, body):
        for post in self.posts:
            if post["id"] == body["id"]:
                post["comments"].append(body)
                return post

    def get_by_username(self, username):
        response = []
        for post in self.posts:
            if post["username"] == username:
                response.append(post)
        response = sorted(response, key=lambda post: post["id"], reverse=True)
        return response

    def get_by_username_filtered(self, username, filters):
        response = []
        for post in self.posts:
            if post["username"] == username:
                if filters["lowerGrade"] <= post["grade"] <= filters["higherGrade"]:
                    response.append(post)
        if filters["sortType"] == "Rating":
            response = sorted(response, key=lambda post: post["rating"], reverse=True)
        elif filters["sortType"] == "Age":
            response = sorted(response, key=lambda post: post["id"], reverse=True)
        return response

    def get_by_id(self, id):
        for post in self.posts:
            if post["id"] == id:
                return post

    def get_all(self):
        response = response = sorted(self.posts, key=lambda post: post["id"], reverse=True)
        return response

    def get_filtered(self, filters):
        response = []
        print(filters)
        for post in self.posts:
            if filters["lowerGrade"] <= post["grade"] <= filters["higherGrade"]:
                response.append(post)
        if filters["sortType"] == "Rating":
            response = sorted(response, key=lambda post: post["rating"], reverse=True)
        elif filters["sortType"] == "Age":
            response = sorted(response, key=lambda post: post["id"], reverse=True)
        return response