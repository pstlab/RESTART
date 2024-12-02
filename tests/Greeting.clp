(defrule Greetings
 (task (task_type Greeting)(pars ?pars)(vals ?vals))
 => (println (str-cat "Greeting" ?pars ?vals))
)