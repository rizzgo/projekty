/**
 * @author xbahni00
 */
import React, { useState, useEffect, useNavigate } from 'react'
import ReactDom from "react-dom";
import Switch from "react-switch";
import "./Comments.css";
import Comment from './Comment';
import { getUsername } from '../Auth/Auth';
import { Navigate } from 'react-router';
import Protected from '../Protected/Protected';




function Comments({ props, open, onClose }) {
    // states
    const [isOpen, setOpen] = useState(true);
    const [comments, setComments] = useState([]);
    const [commentData, setCommentData] = useState('');
    

    async function loadComments() {
        try {
            const response = await fetch(`http://127.0.0.1:8000/posts`, {})
            if (response.ok) {
                const data = await response.json()
                setComments(data[props.post.id].comments)
            }
        }
        catch(err) {
            console.error(err)
        }
    }

    // on prop change set shown comments to stored comments
    useEffect(() => {
        setComments(props.post.comments)
    }, [props])


    // load comments from event
    function loadComment(event){
        setCommentData(event.target.value)
    }

    // add comments to server
    const handleAddComment = async (event)  =>  {
        event.preventDefault()
        if (commentData === "") return
        
        let username = getUsername()
        
        if(username === null) {
            console.log("user not logged in")
            document.getElementById("commentInput").setAttribute(
                "placeholder", "Please log-in to add commments"
                )
                return
            }
            
            let commentText = commentData
            
        let comment = {
            id: props.post.id,
            username: username,
            content: commentText
        }

        const requestOptions = {
            method: 'POST',
            body: JSON.stringify(comment),
            headers: { "Content-type":"application/json", "authorization": username },
        };

        try {
            const response = await fetch(`http://127.0.0.1:8000/posts/comment`, requestOptions)
            if (response.ok) {
                const data = await response.json()
                setComments(data.comments)
                setCommentData('')
            }
        }
        catch(err) {
            console.error(err)
        }
        
        console.log("adding comm")
    }

    if (!open) return null;

    return (
        <div className="commentsStyle">
            <div className="commentGroupStyle">
                {comments.map(comment => (
                    <Comment comment={comment}/>
                ))}
            </div>
            <form onSubmit={handleAddComment}>
                <input id="commentInput" onChange={loadComment} className="addCommentInput" placeholder="Type your comment here" value={commentData}/>
                <button type="submit" className="addCommButton">Add</button>
            </form>
        </div>
    )
}

export default Comments;
