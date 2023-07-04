/**
 * @author xseipe00
 */

import React, { useState, useEffect } from "react";
import Profile from "../profilePage/Profile";
import Home from "../Home/Home";
import Post from "../Post/Post";
import { useParams, useLocation } from "react-router-dom";
import { getUsername } from "../Auth/Auth";

export default function User() {
    // states
    const [posts, setPosts] = useState([]);
    const [user, setUser] = useState("")

    const params = useParams()
    const location = useLocation()

    // load user posts from server
    async function loadPosts(username) {
        try {
            const response = await fetch(`http://127.0.0.1:8000/posts/${username}${location.search}`, {})
            if (response.ok) {
                const data = await response.json()
                console.log("user posts", data)
                setPosts(data)
            }
        }
        catch(err) {
            console.error(err)
        }
    }

    // load posts of user
    useEffect(() => {
        let username = getUsername()
        if (params.username) {
            console.log("params", params, getUsername())
            username = params.username
        }
        loadPosts(username)
        setUser(username)
    }, [params, location.search])
    
    useEffect(()=> {
        console.log("updating posts")
    }, [posts])

    return(
        <React.Fragment>
            <Profile username={user}/>
            <div className="postGroupStyle">
                {posts.map(post => (
                    <Post post={post}/>
                ))}
            </div>
        </React.Fragment>
    )
}