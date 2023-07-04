/**
 * @author xmikus18
 */

import React, { useState, useEffect } from "react";
import { useLocation } from "react-router-dom";

import Post from "../Post/Post";

export default function Home() {

    const [posts, setPosts] = useState([])

    const location = useLocation()

    //loading posts from server into data
    async function loadPosts() {
        try {
            console.log("search params", location.search)
            const response = await fetch(`http://127.0.0.1:8000/posts${location.search}`, {})
            if (response.ok) {
                const data = await response.json()
                console.log("home posts", data)
                setPosts(data)
            }
        }
        catch (err) {
            console.error(err)
        }
    }

    // loading post on location.search change
    useEffect(() => {
        loadPosts()
    }, [location.search])

    return(
        <div className="wrapper">
            {posts.map(post => (
                <Post post={post}/>
            ))}
        </div>
    )
}