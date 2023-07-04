/**
 * @author xmikus18
 */

import React, {useEffect, useState} from 'react';
import {AiFillStar} from 'react-icons/ai'
import {BiUpvote} from 'react-icons/bi'
import {BiDownvote} from 'react-icons/bi'
import {BiRightArrow} from 'react-icons/bi'
import {BiLeftArrow} from 'react-icons/bi'
import { BsChevronCompactLeft, BsChevronCompactRight } from "react-icons/bs";
import {GoCommentDiscussion} from 'react-icons/go'
import './Post.css';
import Comments from './Comments';
import { Link } from "react-router-dom";

import { getUsername } from '../Auth/Auth';
import Login from '../Login/Login';

function Post(props)
{
    
    /*
    let name = "Arnoldino Uzivatel"
    let grade = "7A"
    let rating = "7.8/10"
    let description = "toto je moj oblubeny spotik ktory rad navstevujem navstevujem toto je moj oblubeny spotik ktory rad navstevujem"
    let images = []
    */

    const upvote = "up"
    const downvote = "down"
    const username = getUsername()

    // states
    const [colorUpvote,setColorUpvote] = useState({})
    const [colorDownvote,setColorDownvote] = useState({})
    const [position, setPosition] = useState(0)
    const [commShown, setCommShown] = useState(false);
    const [voted,setVoted] = useState(username in props.post.votes)
    const [rating, setRating] = useState(props.post.rating)

    // on change of props update votes
    useEffect(()=>{
        console.log("votes", props.post.description ,props.post.votes)
        if (username in props.post.votes) {
            if (props.post.votes[username] == upvote) {
                setColorUpvote({color:"green"})    
                setColorDownvote({})
            }    
            else if (props.post.votes[username] == downvote) {
                setColorDownvote({color:"red"})    
                setColorUpvote({})
            }   
        }
        else {
            setColorUpvote({})    
            setColorDownvote({})
        }
        setRating(props.post.rating)
    }, [props])
    

    function handleUpvote(event)
    {
        if(username === null) {
            return
        }

        if (!voted)
        {
            console.log("voted")
            setColorUpvote({color:"green"})
            storeVote(props.post.id, upvote)
        }
    }

    function handleDownvote(event)
    {
        if(username === null) {
            return
        }
        if(!voted)
        {
            console.log("voted")
            setColorDownvote({color:"red"})
            storeVote(props.post.id, downvote)
        }
    }

    // sends votes to server
    async function storeVote(id, vote) {
        try {
            const response = await fetch(`http://127.0.0.1:8000/posts/vote`, {
                method: "POST",
                headers: {
                    "Authorization": username, 
                    "Content-type":"application/json" 
                },
                body: JSON.stringify({"id": id, "vote": vote})
            })
            if (response.ok) {
                let post = await response.json()
                console.log("vote stored", id, vote)
                setVoted(true)
                setRating(post.rating)
            }
        }
        catch (err) {
            console.error(err)
        }
    }

    // sliding through pictures in a post
    function showNextPicture()
    {
        console.log("click ++")
        if (position < props.post.images.length - 1) {
            setPosition(position + 1)
        }
    }
    function showPreviousPicture()
    {
        console.log("click --")
        if (0 < position) {
            setPosition(position - 1)
        }
    }

    function handleOpenUserButton() {
        console.log("handle user button")
        
    }

    return(
        <div id={props.post.id} className="postWindow">
            
            <div className = "upperBar">
                <Link to={`/user/${props.post.username}`}>
                    <button 
                        className="name" 
                        onClick={handleOpenUserButton}>
                            {props.post.username}
                    </button>
                </Link>

                <div className = "difficulity">
                    {props.post.grade}
                </div>
            </div>

            
            <div className="boulderPhoto">
                <img src={"data:image/png;base64," + props.post.images[position]} alt={props.post.images[position]} useMap="#post-buttons" className="photo"/> 
                {(0 < position) ? <div className="previous" onClick={showPreviousPicture}><BsChevronCompactLeft/></div> : null}
                {(position < props.post.images.length - 1) ? <div className="next" onClick={showNextPicture}><BsChevronCompactRight/></div> : null}
            </div>      


            <div className="lowerBar">
                <div className='votes'>
                    <div className='up' onClick={handleUpvote}><a><BiUpvote style={colorUpvote}></BiUpvote></a></div>
                    <div className='down' onClick={handleDownvote}><a><BiDownvote style={colorDownvote}></BiDownvote></a></div>
                </div>
                
                <div className='rating'>
                    <div className='value'>{rating}</div>
                    <div className='star'><a><AiFillStar></AiFillStar></a></div>
                </div>
            
                <div className='commentSection'>
                    <div className='descriptionOfpicture'>{props.post.description}</div>
                    <div className='bubble'>
                        <a onClick={() => setCommShown(!commShown)}>
                            <GoCommentDiscussion className='bubbleLine'></GoCommentDiscussion>
                        </a>
                    </div>
                    <Comments props={props} open={commShown} onClose={() => setCommShown(false)}></Comments>
                </div>
            </div> 
        </div>
        
    );
}

export default Post;