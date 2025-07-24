(defrule exercise_done
  (Robot_has_current_exercise (item_id ?robot) (current_exercise ?exercise))
  (Robot_has_current_level (item_id ?robot) (current_level ?level))
  (Robot_has_current_score (item_id ?robot) (current_score ?score))
  (Robot_has_current_user (item_id ?robot) (current_user ?user))
=>
  (printout t "User '" ?user "' has completed exercise '" ?exercise "' at level " ?level " with score " ?score crlf)
  (facts)
  (do-for-fact ((?ex-done-user ExerciseDone_user) (?ex-done-exercise ExerciseDone_exercise) (?ex-done-done ExerciseDone_done) (?ex-done-level ExerciseDone_level) (?ex-done-score ExerciseDone_score))
    (and
        (eq ?ex-done-user:item_id ?ex-done-exercise:item_id)
        (eq ?ex-done-user:item_id ?ex-done-done:item_id)
        (eq ?ex-done-user:user ?user)
        (eq ?ex-done-exercise:exercise ?exercise))
    (bind ?done (+ ?ex-done-done:done 1))
    (printout t "User '" ?user "' has completed exercise '" ?exercise "' " ?done " times" crlf)
    (set_properties ?ex-done-level:item_id (create$ level) (create$ ?level))
    (set_properties ?ex-done-score:item_id (create$ score) (create$ ?score))
    (set_properties ?ex-done-done:item_id (create$ done) (create$ ?done))
  )
  ; Remove the completed exercise from the list
  (do-for-fact ((?ex exercise)) (not (any-factp ((?ex2 exercise)) (< ?ex2:id ?ex:id))) (retract ?ex))
  (if (any-factp ((?next-ex exercise)) (not (any-factp ((?ex2 exercise)) (< ?ex2:id ?next-ex:id))))
    then
      (do-for-fact ((?next-ex exercise)) (not (any-factp ((?ex2 exercise)) (< ?ex2:id ?next-ex:id)))
        (printout t "Executing exercise '" ?next-ex:type "' at level " ?next-ex:level crlf)
        (add_data ?robot (create$ current_exercise current_level current_score) (create$ ?next-ex:type ?next-ex:level nil))
      )
    else
      (printout t "All exercises completed for user '" ?user "'" crlf)
      (add_data ?robot (create$ current_command) (create$ goodbye))
  )
)