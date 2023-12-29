(deftemplate temperature (slot temp (type NUMBER)))

(defrule start_solver_when_idle
    (solver (solver_ptr ?sp) (state idle))
    =>
    (start_execution ?sp)
)

(defrule delete_solver_when_finished
    (solver (solver_ptr ?sp) (state finished))
    =>
    (delete_solver ?sp)
)

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
        (bind ?temp (nth$ 3 ?data))
        (if (any-factp ((?tf temperature)) TRUE)
            then
            (do-for-fact ((?tf temperature)) TRUE (modify ?tf (temp ?temp)))
            else
            (assert (temperature (temp ?temp)))
        )
        (if (and (not (any-factp ((?slv solver)) (eq ?slv:solver_type environment))) (or (<= ?temp 18) (>= ?temp 33)))
            then
            (adapt_script (get_environment_solver) (str-cat "fact temp = new thermostat.Temperature(start: 0.0, end: 10.0, temp: " (float ?temp) "); goal comf = new thermostat.Comfort();"))
        )
    )
)