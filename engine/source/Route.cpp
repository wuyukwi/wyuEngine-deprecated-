/*
   Demo Name:  Game Project 12
      Author:  Allen Sherrod
     Chapter:  Chapter 13
*/

#include"Route.h"
#include<windows.h>
#include<mmsystem.h>

CPath::CPath() : m_type(0), m_start(0), m_total(0), m_next(0)
{
}

CPath::CPath(int Type, Vector3f start, Vector3f cnt1, Vector3f cnt2, Vector3f end)
{
    m_start = 0;
    m_total = 0;
    m_next = 0;

    SetPath(Type, start, cnt1, cnt2, end);
}

CPath::~CPath()
{
    // Release all resources.
    Shutdown();
}

void CPath::SetPath(int Type, Vector3f start, Vector3f cnt1, Vector3f cnt2, Vector3f end)
{
    // Simple initialize.
    m_type = Type;
    m_startPos = start;
    m_control1 = cnt1;
    m_control2 = cnt2;
    m_endPos = end;
}

void CPath::Shutdown()
{
    // Release resources.
    if (m_next)
    {
        // By calling the m_next pointer's Shutdown, deletion of list of nodes will
        // not start until you get to the end of the list.  This way there are
        // absolutely no memory leaks and is quite safe.
        m_next->Shutdown();
        delete m_next;
        m_next = 0;
    }
}

CRoute::CRoute()
{
    // Initialize variables.
    m_path = 0;
    m_startTime = 0;
}

CRoute::~CRoute()
{
    // Release all resources.
    Shutdown();
}

bool CRoute::AddPath(int Type, Vector3f start, Vector3f cnt1, Vector3f cnt2, Vector3f end)
{
    CPath* ptr;

    // Ok first we test if m_path have already been allocated.  If not then we must do so.
    if (m_path == 0)
    {
        // Allocate data for the root node.
        m_path = new CPath(Type, start, cnt1, cnt2, end);

        // Make sure all went well.
        if (m_path == 0) return false;

        // Since this is the start node, its start m_total is 0.
        m_path->m_start = 0;

        // Remember from the staight path and curve path demos?  Well depending
        // on which m_type we are making will depend on how we get the m_total.
        if (Type == STRIAGHT_PATH)
        {
            m_path->m_total = Vector3f(start - end).length();
        }
        else if (Type == CURVE_PATH)
        {
            float Length01 = Vector3f(cnt1 - start).length();
            float Length12 = Vector3f(cnt2 - cnt1).length();
            float Length23 = Vector3f(end - cnt2).length();
            float Length03 = Vector3f(end - start).length();
            m_path->m_total = (Length01 + Length12 + Length23) * 0.5f + Length03 * 0.5f;
        }
        else
            return false;
    }
    else
    {
        // We will get here only if we are adding the 2 or more path.  First up we
        // will get a pointer to the root node so we can use it to move through
        // the link list.
        ptr = m_path;

        // Now we will move through the list until we get to the last one.  This
        // can be determined by if the m_next pointer in the current node is NULL.
        // If so then we are in the last node.
        while (ptr->m_next != 0)
        {
            ptr = ptr->m_next;
        }

        // Create the m_next path in our list.
        ptr->m_next = new CPath(Type, start, cnt1, cnt2, end);

        // Error checking.
        if (ptr->m_next == 0) return false;

        // This start m_total is determined by the full m_total of the previous path.
        ptr->m_next->m_start = ptr->m_total + ptr->m_start;;

        // Just like above, depending on the m_type of path will depend on the m_total.
        if (Type == STRIAGHT_PATH)
        {
            ptr->m_next->m_total = Vector3f(start - end).length();
        }
        else if (Type == CURVE_PATH)
        {
            float Length01 = Vector3f(cnt1 - start).length();
            float Length12 = Vector3f(cnt2 - cnt1).length();
            float Length23 = Vector3f(end - cnt2).length();
            float Length03 = Vector3f(end - start).length();
            ptr->m_next->m_total = (Length01 + Length12 + Length23) * 0.5f + Length03 * 0.5f;
        }
        else
            return false;
    }

    return true;
}

void CRoute::GetPosition(float time, Vector3f& objPos)
{
    CPath* ptr = m_path;

    // Error checking.
    if (m_path == 0) return;

    // Initialize the start time if it has not been already.
    if (m_startTime == 0) m_startTime = (float)timeGetTime();

    // Loop through each path to see where this object is located at.
    do
    {
        // Check if the distance we are at in this route falls within this path.
        if (time >= ptr->m_start && time < ptr->m_start + ptr->m_total)
        {
            // If this is the right path then we calculate the distance traveled by
            // subtracting from the start m_total of the path.  This will give us
            // where on the paths m_total we are.
            time -= ptr->m_start;

            // Determine what the scalar should be.  This will help determine the new position.
            // We use the path's max m_total to get the exact position on the lenght scale
            // we are at.
            float Scalar = time / ptr->m_total;

            // Depending on the m_type of path this is we are either calculating on a striaght
            // path or on a curve.
            if (ptr->m_type == 0)
            {
                objPos = (CalcStriaghtPath(ptr->m_startPos, ptr->m_endPos, Scalar));
            }
            else
                objPos = (CalcBezierCurvePos(ptr->m_startPos, ptr->m_control1, ptr->m_control2,
                    ptr->m_endPos, Scalar));

            // There is no need to keep going through the link list because we found
            // which path we are moving on so we break out.
            break;
        }
        else
        {
            // If this was the last node (path) and our position does not travel through
            // it, we can reset the animation by resetting m_startTime.  If you don't want
            // the animation to loop then comment this line.
            if (ptr->m_next == 0) m_startTime = (float)timeGetTime();
        }

        // Move to the m_next path so we can check it.  If there is nothing m_next then
        // ptr will = 0, breaking us out of the loop.
        ptr = ptr->m_next;
    } while (ptr != 0);

    return;
}

Vector3f CRoute::CalcBezierCurvePos(Vector3f start, Vector3f c1, Vector3f c2, Vector3f end, float Scalar)
{
    // This function was taken out of the curve path demo.
    Vector3f out;

    out = start * (1.0f - Scalar) * (1.0f - Scalar) * (1.0f - Scalar) +
        c1 * 3.0f * Scalar * (1.0f - Scalar) * (1.0f - Scalar) +
        c2 * 3.0f * Scalar * Scalar * (1.0f - Scalar) +
        end * Scalar * Scalar * Scalar;

    return out;
}

Vector3f CRoute::CalcStriaghtPath(Vector3f start, Vector3f end, float Scalar)
{
    // This function was taken out of the Striaght line path demo.
    Vector3f out = (end - start) * Scalar + start;

    return out;
}

void CRoute::Shutdown()
{
    // Release all resources.
    if (m_path)
    {
        // Call the root path node Shutdown() will clear out the whole list.
        m_path->Shutdown();
        delete m_path;
        m_path = 0;
    }
}