;Regola per creare lo stato dell'utente e creare il pianificazione (Stato Reasoning e Idle)

(defrule user_state_rule
    (User_physical_impairment (item_id ?user)(physical_impairment ?physical_impairment))
    (User_cognitive_impairment (item_id ?user)(cognitive_impairment ?cognitive_impairment))
    (User_personality_profile (item_id ?user)(personality_profile ?personality_profile))
    (User_daily_routine (item_id ?user)(daily_routine ?daily_routine))

=>

    (println "goal user1 = new r.Greeting(id: \"" ?user "\");" )
    (println "fact user1 = new r.UserState(id: \"" ?user "\", physical_impairment: " (lowcase (str-cat ?physical_impairment))", personality_profile: \"" ?personality_profile"\", cognitive_impairment: \""?cognitive_impairment"\");")
    (bind ?solver_id (new_solver_rules User_Well_Being (create$ "user_well_being_del")))
    (adapt_script ?solver_id (str-cat "fact user1 = new r.UserState(id: \"" ?user "\", physical_impairment: " (lowcase (str-cat ?physical_impairment))", personality_profile: \"" ?personality_profile"\", cognitive_impairment: \""?cognitive_impairment"\");"))
    (adapt_script ?solver_id ?daily_routine) 
    (adapt_script ?solver_id (str-cat "goal user1 = new r.Greeting(id: \"" ?user "\");")) 
    (adapt_script ?solver_id (str-cat "goal user = new r.Cognitive_Exercise(id: \"" ?user "\");")) 

)


    
















 


