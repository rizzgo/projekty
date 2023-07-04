/**
 * @author xbahni00
 */
import React, { useState, useEffect } from 'react'
import "./Comment.css";


function Comment({comment}) {
    return (
        <div className="commentContainer">
            <dt className="username">{comment.username}</dt><dd className="content">{comment.content}</dd>
        </div>
    )
}

export default Comment;
