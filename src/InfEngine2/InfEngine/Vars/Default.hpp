#ifndef __Vars_Default_hpp__
#define __Vars_Default_hpp__

/** Автоматические внутренние и стандартизованные переменные переменные. */

/** InfPerson - переменная, определяющая принадлежность шаблона к инфу. */
#define InfEngineVarInfPerson               Vars::Id{ 0, false }

/** Reply- - предыдущий ответ инфа. */
#define InfEngineVarInfPrevResponse         Vars::Id{ 1, false }

/** Reply-- - предпоследний ответ инфа. */
#define InfEngineVarInfPrevPrevResponse     Vars::Id{ 2, false }

/** Query- - предыдущий запрос пользователя. */
#define InfEngineVarUserPrevRequest         Vars::Id{ 3, false }

/** Query-- - предпоследений запрос пользователя. */
#define InfEngineVarUserPrevPrevRequest     Vars::Id{ 4, false }

/** ThatAnchor - якорь для соединения шаблонов. */
#define InfEngineVarThatAnchor              Vars::Id{ 5, false }

/** LastMark - переменная, которая очищается после каждого ответа инфа, если ответ не содержит иную инструкцию. */
#define InfEngineVarLastMark                Vars::Id{ 6, false }

/** AutoReplyId - идентификатор шаблона, на основании которого был построен ответ инфа. */
#define InfEngineVarAutoReplyId             Vars::Id{ 7, false }

/** AutoDate - дата на начало обработки запроса пользователя. */
#define InfEngineVarAutoDate                Vars::Id{ 8, false }

/** AutoTime - время начала обработки запроса пользователя. */
#define InfEngineVarAutoTime                Vars::Id{ 9, false }

/** InfEllipsisMode - флаг, управляющий логикой раскрытия эллипсиса для инфа. */
#define InfEngineVarInfEllipsisMode        Vars::Id{ 10, false }

/** AutoEllipsisDisclosed - раскрытый эллипсис, если его удалось раскрыть. */
#define InfEngineVarAutoEllipsisDisclosed  Vars::Id{ 11, false }

/** AutoResponseType - тип формирования ответа инфа. */
#define InfEngineVarAutoResponseType       Vars::Id{ 12, false }

/** AutoEllipsisId - идентификатор сработавшего шаблона эллипсисов. */
#define InfEngineVarAutoEllipsisId         Vars::Id{ 13, false }

/** AutoEllipsisDetected - идентификаторы частично раскрытых эллипсисов. */
#define InfEngineVarAutoEllipsisDetected   Vars::Id{ 14, false }

/** InfEngineVarAnimation - анимация эмоций инфа. */
#define InfEngineVarAnimation              Vars::Id{ 15, false }

/** AutoResponseNumber - счетчик количества итераций в диалоге. */
#define InfEngineVarAutoResponseCounter    Vars::Id{ 16, false }

/** LastMark2 - вторая переменная, которая очищается после каждого ответа инфа, если ответ не содержит иную инструкцию. */
#define InfEngineVarLastMark2              Vars::Id{ 17, false }

/** AcvInfo - переменная для сбора статистики по темам, проставленным в лингвистических данных. */
#define InfEngineVarAcvInfo                Vars::Id{ 18, false }

/** RequestLength - переменная, содержащая длину запроса пользователя в словах. */
#define InfEngineVarRequestLength          Vars::Id{ 19, false }

/** Inf_UUID - переменная, содержащая UUID инфа. */
#define InfEngineVarInfUUID                Vars::Id{ 20, false }

/** Inf_CUID - переменная, содержащая CUID инфа. */
#define InfEngineVarInfCUID                Vars::Id{ 21, false }

/** User_TimeZone - переменная, содержащая разницу во времени с Москвой в формате MSK{+|-}NUMBER[:NUMBER]. */
#define InfEngineVarUserTimeZone           Vars::Id{ 22, false }

/** Last_Service - переменная, аналогичная переменной LastMark. */
#define InfEngineVarLastService            Vars::Id{ 23, false }

/** Last_Set1 - переменная, аналогичная переменной LastMark. */
#define InfEngineVarLastSet1               Vars::Id{ 24, false }

/** Last_Set2 - переменная, аналогичная переменной LastMark. */
#define InfEngineVarLastSet2               Vars::Id{ 25, false }

/** Last_Set3 - переменная, аналогичная переменной LastMark. */
#define InfEngineVarLastSet3               Vars::Id{ 26, false }

/** Last_Set4 - переменная, аналогичная переменной LastMark. */
#define InfEngineVarLastSet4               Vars::Id{ 27, false }

/** Last_Set5 - переменная, аналогичная переменной LastMark. */
#define InfEngineVarLastSet5               Vars::Id{ 28, false }

/** Last_Set6 - переменная, аналогичная переменной LastMark. */
#define InfEngineVarLastSet6               Vars::Id{ 29, false }

/** Match_Weight - переменная, в которую сохраняется вес ответа. */
#define InfEngineVarMatchWeight            Vars::Id{ 30, false }

/** LangDetectedKazakh - переменная, которая принимает значение TRUE, если в запросе пользователя обнаружены символы казахского алфавита. */
#define InfEngineVarLangDetectedKazakh     Vars::Id{ 31, false }

/** InputType - переменная, содержащая значение KEYBOARD, если запрос инфу был введён с помощью клавиатуры, или VOICE, если использовался голосовой ввод. */
#define InfEngineVarInputType              Vars::Id{ 32, false }

/** SkipUnknown - переменная, содержащая флаг необходимости повышения приоритета ответов без флага unknown из основной базы над приоритетом ответов из
 * пользовательской базы. */
#define InfEngineSkipUnknown               Vars::Id{ 33, false }

/** DictMergeMethod - переменная, переопределяющая для конкретного инфа одноимённую настройку InfServer. Задаёт метод объединения пользовательских и
 * основных словарей. ["SUBSTITUTION", "ADDICTION"]. */
#define InfEngineDictMergeMethod           Vars::Id{ 34, false }

/** DictMergeEffect - переменная, переопределяющая для конкретного инфа одноимённую настройку InfServer. Показывает, нужно ли использовать пользовательские
словари при поиске по шаблонам основной базы. ["TRUE", "FALSE"]. */
#define InfEngineDictMergeEffect           Vars::Id{ 35, false }

/** AutoReplyLabel - метка шаблона, на основании которого был построен ответ инфа. */
#define InfEngineVarAutoReplyLabel         Vars::Id{ 36, false }

/** AutoRequestCounter - число запросов в сессии до текущего. */
#define InfEngineVarAutoRequestCounter        Vars::Id{ 37, false }

/** AutoEventIds - идентификаторы событий, произошедших с момента последнего запроса. */
#define InfEngineVarAutoEventIds            Vars::Id{ 38, false }

/** DisableCache - переменная, сигнализирующая о том, что результат обработки запроса не должен попадать в кэш. */
#define InfEngineVarDisableCache            Vars::Id{ 39, false }

#endif /** __Vars_Default_hpp__ */
