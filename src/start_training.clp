(defrule start_training
  (Robot_has_command_completed (item_id ?robot) (command_completed rot))
  (Robot_has_current_user (item_id ?robot) (current_user ?user))  
=>
  (add_data ?robot (create$ current_command) (create$ training))
  (do-for-all-facts ((?domain CognitiveDomain_name)) TRUE
    (printout t "Cognitive Domain: " ?domain:name crlf)
    (do-for-all-facts ((?ex-done-user ExerciseDone_user) (?ex-done-exercise ExerciseDone_exercise) (?ex-done-done ExerciseDone_done) (?cog-ex-domain CognitiveExercise_domain))
      (and
        (eq ?ex-done-user:item_id ?ex-done-exercise:item_id)
        (eq ?ex-done-user:item_id ?ex-done-done:item_id)
        (eq ?ex-done-exercise:exercise ?cog-ex-domain:item_id)
        (eq ?ex-done-user:user ?user)
        (eq ?cog-ex-domain:domain ?domain:item_id))
      then
        (printout t "User '" ?user "' has done exercise '" ?ex-done-user:item_id "' in domain '" ?domain:name "' " ?ex-done-done:done " times" crlf)
    )
    ; Check if there are any exercises done by the user in the current domain
    (if (not (any-factp ((?ex-done-user ExerciseDone_user) (?ex-done-exercise ExerciseDone_exercise) (?ex-done-done ExerciseDone_done) (?cog-ex-domain CognitiveExercise_domain))
    (and
      (eq ?ex-done-user:item_id ?ex-done-exercise:item_id)
      (eq ?ex-done-user:item_id ?ex-done-done:item_id)
      (eq ?ex-done-exercise:exercise ?cog-ex-domain:item_id)
      (eq ?ex-done-user:user ?user)
      (> ?ex-done-done:done 0)
      (eq ?cog-ex-domain:domain ?domain:item_id))))
      then
        (printout t "User '" ?user "' has not done any exercises in domain '" ?domain:name "' yet" crlf)
        ; iterate over all the tests done by the user in the given domain and get the lowest score
        (bind ?min-score 4)  ; Initialize with high value
        (do-for-all-facts ((?test-done-user TestDone_user) (?test-done-test TestDone_test) (?test-done-score TestDone_score) (?cog-test-domain CognitiveTest_domain)) 
          (and (eq ?test-done-user:item_id ?test-done-test:item_id)
               (eq ?test-done-user:item_id ?test-done-score:item_id)
               (eq ?test-done-user:user ?user)
               (eq ?test-done-test:test ?cog-test-domain:item_id)
               (eq ?cog-test-domain:domain ?domain:item_id))
          (printout t "User '" ?user "' has done test '" ?test-done-test:item_id "' with score " ?test-done-score:score crlf)
          (if (< ?test-done-score:score ?min-score)
            then
              (bind ?min-score ?test-done-score:score)
          )
        )
        (printout t "User's score in '" ?domain:name "' is: " ?min-score crlf)
        ; Get an exercise type that is available in the domain
        (do-for-fact ((?cog-ex-name CognitiveExercise_name) (?cog-ex-domain CognitiveExercise_domain)) (and (eq ?cog-ex-name:item_id ?cog-ex-domain:item_id) (eq ?cog-ex-domain:domain ?domain:item_id))
          (printout t "Exercise '" ?cog-ex-name:name "' is available in domain '" ?domain:name "'" crlf)
          (if (eq ?min-score 0)
            then
              (enqueue-exercise ?cog-ex-name:item_id 1)
              (enqueue-exercise ?cog-ex-name:item_id 1)
          )
          (if (eq ?min-score 1)
            then
              (enqueue-exercise ?cog-ex-name:item_id 2)
              (enqueue-exercise ?cog-ex-name:item_id 2)
          )
          (if (> ?min-score 1)
            then
              (enqueue-exercise ?cog-ex-name:item_id 3)
              (enqueue-exercise ?cog-ex-name:item_id 3)
              (enqueue-exercise ?cog-ex-name:item_id 3)
              (enqueue-exercise ?cog-ex-name:item_id 3)
          )    
        )
      else
        (printout t "User '" ?user "' has already done exercises in domain '" ?domain:name "'" crlf)
    )
  )
  ; Execute the first available exercise
  (do-for-fact ((?ex exercise)) (not (any-factp ((?ex2 exercise)) (< ?ex2:id ?ex:id)))
    (printout t "Executing exercise " ?ex:type " at level " ?ex:level crlf)
    (add_data ?robot (create$ current_exercise current_level) (create$ ?ex:type ?ex:level))
  )
)