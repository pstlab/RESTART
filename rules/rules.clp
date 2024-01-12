(deftemplate environment_state (slot temperature (type NUMBER)))
(deftemplate user_state (slot emotion (type SYMBOL)))

(deffacts initial
    (environment_state (temperature 20)
    (user_state (emotion happy))
)

(defrule start_solver_when_idle (solver (solver_ptr ?sp) (state idle)) => (start_execution ?sp))
(defrule delete_solver_when_finished (solver (solver_ptr ?sp) (state finished)) => (delete_solver ?sp))

(deffunction get_environment_solver ()
    (if (any-factp ((?slv solver)) (eq ?slv:solver_type environment))
        then
        (do-for-fact ((?slv solver)) (eq ?slv:solver_type environment) (return ?slv:solver_ptr))
        else
        (println "Creating new `environment` solver")
        (return (new_solver_files environment (create$ "rules/environment.rddl")))
    )
)

(deffunction sensor_data (?sensor ?sensor_type ?time ?data)
    (if (eq (fact-slot-value ?sensor_type name) "sensor_node")
        then
        (bind ?humidity (nth$ 1 ?data))
        (bind ?pressure (nth$ 2 ?data))
        (bind ?temperature (nth$ 3 ?data))
        (do-for-fact ((?es environment_state)) TRUE (modify ?es (temperature ?temperature)))
        (if (and (not (any-factp ((?slv solver)) (eq ?slv:solver_type environment))) (or (<= ?temperature 18) (>= ?temperature 33)))
            then
            (adapt_script (get_environment_solver) (str-cat "fact temp = new thermostat.Temperature(start: 0.0, end: 10.0, temp: " (float ?temperature) "); goal comf = new thermostat.Comfort();"))
        )
    )
    (if (eq (fact-slot-value ?sensor_type name) "prompt")
        then
        (bind ?message (nth$ 1 ?data))
        (understand (sym-cat 42) ?message)
    )
    (if (eq (fact-slot-value ?sensor_type name) "emotion")
        then
        (bind ?emotion (nth$ 1 ?data))
        (do-for-fact ((?us user_state)) TRUE (modify ?us (emotion (sym-cat ?emotion))))
    )
)

(deffunction ending (?solver_ptr ?id)
    (do-for-fact ((?tsk task) (?es environment_state)) (and (eq ?tsk:id ?id) (eq ?tsk:task_type Heating) (<= ?es:temperature 18)) (return FALSE))
    (do-for-fact ((?tsk task) (?es environment_state)) (and (eq ?tsk:id ?id) (eq ?tsk:task_type Cooling) (>= ?es:temperature 33)) (return FALSE))
    (return TRUE)
)

(deffunction intent (?name ?confidence, ?entities, ?values)
    (do-for-fact ((?us user_state)) TRUE
        (if (and (eq ?name greet) (eq ?us:emotion happy)) then (say web "Ciao! Come posso aiutarti?") (return))
        (if (and (eq ?name greet) (eq ?us:emotion sad)) then (say web "Ciao! Va tutto bene?") (return))
        (if (eq ?name goodbye) then (say web "Arrivederci!") (return))
    )
    (say web "Non ho capito")
)