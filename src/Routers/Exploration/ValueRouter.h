#ifndef VALUEROUTER_H
#define VALUEROUTER_H

#include <QMainWindow>
#include "AStarRouter.h"
#include "MissionScanner.h"
#include "SystemEntryCoordinateResolver.h"
#include "ui_ValueRouter.h"
#include "AbstractBaseWindow.h"
#include "TSPWorker.h"
#include "CommanderInfo.h"

class ValueRouter : public AbstractBaseWindow<Ui::ValueRouter> {
Q_OBJECT

public:
    ValueRouter(QWidget *parent, AStarRouter *router, SystemList *systems);

    virtual ~ValueRouter();

protected slots:
    void updateFilters() override;
    void routeCalculated(const RouteResult &route) override;
    void onRouterCreated(TSPWorker *worker) override;

    void handleEvent(const JournalFile &file, const Event &ev);
    void scanJournals();
    void updateSystem();
    void saveSettings() const override;

private:
    void restoreSettings() const;

    QMap<QString, QSet<QString>> _commanderExploredSystems;
    SystemEntryCoordinateResolver *_systemResolverDestination;

};

#endif // VALUEROUTER_H