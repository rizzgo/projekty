/**
 * @author xmikus18
 */

import React, { useState, useEffect} from "react";
import { Outlet, Link, useNavigate , useLocation} from "react-router-dom";
import { CgMathPlus } from "react-icons/cg"
import { BiFilterAlt, BiHome, BiUser } from "react-icons/bi"

import "./Layout.css"
import Filter from "../Filter/Filter";

export default function Layout() {

    //states for filter info
    const [filterOpen, setFilterOpen] = useState(false)
    const [filterData, setFilterData] = useState()
    const navigate = useNavigate()
    // const [loginOpen, setLoginOpen] = useState(false);
    // const [registerOpen, setRegisterOpen] = useState(false);

    // handling close of filter
    const handleFilterClose = () => {
        console.log(filterData)
        setFilterOpen(false)
    }

    const location = useLocation()

    useEffect(() => {
        console.log(location.pathname)
        if (location.pathname === "/")
        {
            navigate("/home")
        }
    })

    return (
        <div className="layout">
            <div className="header">
                <div className="logo"><Link to="home">Boulder.io</Link></div>
                <div className="newPost"><Link to="new"><CgMathPlus color="white" /></Link></div>
            </div>
            <div className="content">
                <Outlet />
            </div>
            <div className="taskBar">
                <Link className="taskBarItem" to="home"><BiHome /></Link>
                <a onClick={() => setFilterOpen(true)} className="taskBarItem filter"><BiFilterAlt /></a>
                
                {/* <a onClick={() => setLoginOpen(true)} className="taskBarItem"><BiUser /></a> */}
                <Link className="taskBarItem" to="user"><BiUser /></Link>
            </div>
            <div>
                <Filter open={filterOpen} onClose={handleFilterClose}></Filter>
            </div>
            {/*<div>
                <Login open={loginOpen} onClose={() => setLoginOpen(false)} onRegister={() => {setLoginOpen(false); setRegisterOpen(true)}}></Login>
            </div>
            <div>
                <Register open={registerOpen} onClose={() => setRegisterOpen(false)} onRegister={() => {setRegisterOpen(false); setLoginOpen(true)}}></Register>
            </div>*/}
        </div>
    )
}