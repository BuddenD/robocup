/*! @file JobList.h
    @brief Declaration of JobList class.
 
    @class JobList
    @brief A class containing the list of jobs to be done by modules
 

    @author Jason Kulk
 
  Copyright (c) 2009 Jason Kulk
 
    This file is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This file is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with NUbot.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef JOBLIST_H
#define JOBLIST_H

#include "Job.h"

#include <list>
#include <iterator>
using namespace std;

class JobListIterator;      // definition is below JobList

class JobList
{
public:
    friend class JobListIterator;
    typedef JobListIterator iterator;
public:
    JobList();
    ~JobList();
    
    // Add job interface
    void addJob(Job* job);
    void addVisionJob(Job* job);
    void addLocalisationJob(Job* job);
    void addBehaviourJob(Job* job);
    void addMotionJob(Job* job);
    void addLightJob(Job* job);
    void addCameraJob(Job* job);
    void addSoundJob(Job* job);
    void addSystemJob(Job* job);
    void addOtherJob(Job* job);
    
    // Remove job interface
    void removeJob(Job* job);
    void removeVisionJob(Job* job);
    void removeLocalisationJob(Job* job);
    void removeBehaviourJob(Job* job);
    void removeMotionJob(Job* job);
    void removeLightJob(Job* job);
    void removeCameraJob(Job* job);
    void removeSoundJob(Job* job);
    void removeSystemJob(Job* job);
    void removeOtherJob(Job* job);
    
    // Iterators over the jobs
    iterator begin();
    iterator end();
    list<Job*>::iterator vision_begin();
    list<Job*>::iterator vision_end();
    list<Job*>::iterator localisation_begin();
    list<Job*>::iterator localisation_end();
    list<Job*>::iterator behaviour_begin();
    list<Job*>::iterator behaviour_end();
    list<Job*>::iterator motion_begin();
    list<Job*>::iterator motion_end();
    list<Job*>::iterator light_begin();
    list<Job*>::iterator light_end();
    list<Job*>::iterator camera_begin();
    list<Job*>::iterator camera_end();
    list<Job*>::iterator sound_begin();
    list<Job*>::iterator sound_end();
    list<Job*>::iterator system_begin();
    list<Job*>::iterator system_end();
    list<Job*>::iterator other_begin();
    list<Job*>::iterator other_end();
    
    void clear();
    
    void summaryTo(ostream& output);
    void csvTo(ostream& output);
    
    ostream& operator<< (ostream& output);
    istream& operator>> (istream& input);
    
private:
    void addJob(Job* job, list<Job*>& joblist);
    void removeJob(Job* job, list<Job*>& joblist);

private:
    list<Job*> m_vision_jobs;               //!< a list of all the current vision jobs
    list<Job*> m_localisation_jobs;         //!< a list of all the current localisation jobs
    list<Job*> m_behaviour_jobs;            //!< a list of all the behaviour jobs
    list<Job*> m_motion_jobs;               //!< a list of all the current motion jobs
    list<Job*> m_light_jobs;                //!< a list of all the current light jobs
    list<Job*> m_camera_jobs;               //!< a list of all the current camera jobs
    list<Job*> m_sound_jobs;                //!< a list of all the current jobs for the sound system
    list<Job*> m_system_jobs;               //!< a list of all the current system/os jobs
    list<Job*> m_other_jobs;                //!< a list of all other jobs
    list<list<Job*>*> m_job_lists;          //!< a list of all the lists of jobs
};


/*! @class JobListIterator
    @brief A stl::iterator over a JobList. It iterates over all elements in the list regards this of their type.
 */
class JobListIterator : public iterator<forward_iterator_tag, Job*>
{
public:
    JobListIterator();
    JobListIterator(JobList* joblist, bool end=false);
    JobListIterator& operator++();
    JobListIterator& operator++(int);
    bool operator==(const JobListIterator& rhs);
    bool operator!=(const JobListIterator& rhs);
    Job* operator*();
private:
    void moveToNextList();
private:
    JobList* m_joblist;
    Job* m_job;                                                 //!< the current job
    list<Job*>::iterator m_list_iterator;                       //!< an iterator over the current lists in m_job_lists
    list<list<Job*>*>::iterator m_job_lists_iterator;           //!< an iterator over the lists in m_job_lists
};

#endif

